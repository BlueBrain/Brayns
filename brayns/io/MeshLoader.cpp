/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "MeshLoader.h"

#ifdef BRAYNS_USE_ASSIMP
#include <assimp/Exporter.hpp>
#include <assimp/IOSystem.hpp> // must come before Exporter.hpp
#include <assimp/Importer.hpp>
#include <assimp/ProgressHandler.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <boost/filesystem.hpp>
#include <brayns/common/log.h>
#include <fstream>
#endif

#include <brayns/common/scene/Model.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/common/utils/Utils.h>

namespace brayns
{
const size_t TOTAL_PROGRESS = 100;
const size_t LOADING_FRACTION = 50;
const size_t POST_PROCESSING_FRACTION = TOTAL_PROGRESS - LOADING_FRACTION;
#ifdef BRAYNS_USE_ASSIMP
class ProgressWatcher : public Assimp::ProgressHandler
{
public:
    ProgressWatcher(Loader& parent, const std::string& filename)
        : _parent(parent)
    {
        _msg << "Loading " << shortenString(filename) << " ..." << std::endl;
    }

    bool Update(const float percentage) final
    {
        _parent.updateProgress(_msg.str(), percentage * LOADING_FRACTION,
                               TOTAL_PROGRESS);
        return true;
    }

private:
    Loader& _parent;
    std::function<void()> _cancelCheck;
    std::stringstream _msg;
};
#endif

MeshLoader::MeshLoader(const GeometryParameters& geometryParameters)
    : _geometryParameters(geometryParameters)
{
}

std::set<std::string> MeshLoader::getSupportedDataTypes()
{
    std::set<std::string> types;
#ifdef BRAYNS_USE_ASSIMP
    std::string extensions;
    Assimp::Importer importer;
    importer.GetExtensionList(extensions);

    std::istringstream stream(extensions);
    std::string s;
    while (std::getline(stream, s, ';'))
    {
        auto pos = s.find_last_of(".");
        types.insert(pos == std::string::npos ? s : s.substr(pos + 1));
    }
#endif
    return types;
}

#ifdef BRAYNS_USE_ASSIMP
void MeshLoader::importFromFile(const std::string& fileName, Scene& scene,
                                const size_t index BRAYNS_UNUSED,
                                const Matrix4f& transformation,
                                const size_t defaultMaterialId)
{
    const boost::filesystem::path file = fileName;
    Assimp::Importer importer;
    importer.SetProgressHandler(new ProgressWatcher(*this, fileName));
    if (!importer.IsExtensionSupported(file.extension().c_str()))
    {
        std::stringstream msg;
        msg << "File extension " << file.extension() << " is not supported";
        throw std::runtime_error(msg.str());
    }

    std::ifstream meshFile(fileName, std::ios::in);
    if (!meshFile.good())
        throw std::runtime_error("Could not open file " + fileName);
    meshFile.close();

    const aiScene* aiScene = importer.ReadFile(fileName.c_str(), _getQuality());

    if (!aiScene)
    {
        std::stringstream msg;
        msg << "Error parsing mesh " << fileName.c_str() << ": "
            << importer.GetErrorString();
        throw std::runtime_error(msg.str());
    }

    if (!aiScene->HasMeshes())
        throw std::runtime_error("Error finding meshes in scene");

    boost::filesystem::path filepath = fileName;
    ModelMetadata metadata = {{"uri", fileName}};
    auto model = scene.createModel(fileName, metadata);
    _postLoad(aiScene, *model, transformation, defaultMaterialId,
              filepath.parent_path().string());
}

void MeshLoader::importFromBlob(Blob&& blob, Scene& scene,
                                const size_t index BRAYNS_UNUSED,
                                const Matrix4f& transformation,
                                const size_t defaultMaterialId)
{
    Assimp::Importer importer;
    importer.SetProgressHandler(new ProgressWatcher(*this, blob.name));

    const aiScene* aiScene =
        importer.ReadFileFromMemory(blob.data.data(), blob.data.size(),
                                    _getQuality(), blob.type.c_str());

    if (!aiScene)
        throw std::runtime_error(importer.GetErrorString());

    if (!aiScene->HasMeshes())
        throw std::runtime_error("No meshes found");

    auto model = scene.createModel(blob.name);
    std::cout << blob.name << std::endl;
    _postLoad(aiScene, *model, transformation, defaultMaterialId);
}

void MeshLoader::_createMaterials(Model& model, const aiScene* aiScene,
                                  const std::string& folder)
{
    BRAYNS_DEBUG << "Loading " << aiScene->mNumMaterials << " materials"
                 << std::endl;

    for (size_t m = 0; m < aiScene->mNumMaterials; ++m)
    {
        const size_t materialId = _getMaterialId(m);
        aiMaterial* aimaterial = aiScene->mMaterials[m];

        aiString valueString;
        aimaterial->Get(AI_MATKEY_NAME, valueString);
        std::string name{valueString.C_Str()};
        auto material = model.createMaterial(m, name);

        struct TextureTypeMapping
        {
            aiTextureType aiType;
            TextureType type;
        };

        const size_t NB_TEXTURE_TYPES = 6;
        TextureTypeMapping textureTypeMapping[NB_TEXTURE_TYPES] = {
            {aiTextureType_DIFFUSE, TextureType::TT_DIFFUSE},
            {aiTextureType_NORMALS, TextureType::TT_NORMALS},
            {aiTextureType_SPECULAR, TextureType::TT_SPECULAR},
            {aiTextureType_EMISSIVE, TextureType::TT_EMISSIVE},
            {aiTextureType_OPACITY, TextureType::TT_OPACITY},
            {aiTextureType_REFLECTION, TextureType::TT_REFLECTION}};

        for (size_t textureType = 0; textureType < NB_TEXTURE_TYPES;
             ++textureType)
        {
            if (aimaterial->GetTextureCount(
                    textureTypeMapping[textureType].aiType) > 0)
            {
                aiString path;
                if (aimaterial->GetTexture(
                        textureTypeMapping[textureType].aiType, 0, &path,
                        nullptr, nullptr, nullptr, nullptr,
                        nullptr) == AI_SUCCESS)
                {
                    const std::string fileName = folder + "/" + path.data;
                    BRAYNS_DEBUG << "Loading texture: " << fileName
                                 << std::endl;
                    material->setTexture(fileName,
                                         textureTypeMapping[textureType].type);
                }
            }
        }

        aiColor4D value4f(0.f);
        float value1f;
        aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, value4f);
        material.setColor(Vector3f(value4f.r, value4f.g, value4f.b));

        value1f = 0.f;
        if (aimaterial->Get(AI_MATKEY_OPACITY, value1f) != AI_SUCCESS)
            material->setOpacity(value4f.a);
        else
            material->setOpacity(fabs(value1f) < 0.01f ? 1.f : value1f);

        value1f = 0.f;
        aimaterial->Get(AI_MATKEY_REFLECTIVITY, value1f);
        material->setReflectionIndex(value1f);

        value4f = aiColor4D(0.f);
        aimaterial->Get(AI_MATKEY_COLOR_SPECULAR, value4f);
        material->setSpecularColor(Vector3f(value4f.r, value4f.g, value4f.b));

        value1f = 0.f;
        aimaterial->Get(AI_MATKEY_SHININESS, value1f);
        material->setSpecularExponent(fabs(value1f) < 0.01f ? 100.f : value1f);

        value4f = aiColor4D(0.f);
        aimaterial->Get(AI_MATKEY_COLOR_EMISSIVE, value4f);
        material->setEmission(value4f.r);

        value1f = 0.f;
        aimaterial->Get(AI_MATKEY_REFRACTI, value1f);
        material->setRefractionIndex(fabs(value1f - 1.f) < 0.01f ? 1.0f
                                                                : value1f);
    }
}

void MeshLoader::_postLoad(const aiScene* aiScene, Model& model,
                           const Matrix4f& transformation,
                           const size_t defaultMaterialId,
                           const std::string& folder)
{
    if (defaultMaterialId == NO_MATERIAL)
        _createMaterials(model, aiScene, folder);

    size_t nbVertices = 0;
    size_t nbFaces = 0;
    std::map<size_t, size_t> indexOffsets;
    for (size_t m = 0; m < aiScene->mNumMeshes; ++m)
    {
        auto mesh = aiScene->mMeshes[m];
        auto& triangleMeshes = model.getTrianglesMeshes()[mesh->mMaterialIndex];

        nbVertices += mesh->mNumVertices;
        triangleMeshes.vertices.reserve(nbVertices);
        if (mesh->HasNormals())
            triangleMeshes.normals.reserve(nbVertices);
        if (mesh->HasTextureCoords(0))
            triangleMeshes.textureCoordinates.reserve(nbVertices);
        for (size_t i = 0; i < mesh->mNumVertices; ++i)
        {
            const auto& v = mesh->mVertices[i];
            const Vector4f vertex =
                transformation * Vector4f(v.x, v.y, v.z, 1.f);
            const Vector3f transformedVertex = {vertex.x(), vertex.y(),
                                                vertex.z()};
            triangleMeshes.vertices.push_back(transformedVertex);
            model.getBounds().merge(transformedVertex);
            if (mesh->HasNormals())
            {
                const auto& n = mesh->mNormals[i];
                const Vector4f normal =
                    transformation * Vector4f(n.x, n.y, n.z, 0.f);
                const Vector3f transformedNormal = {normal.x(), normal.y(),
                                                    normal.z()};
                triangleMeshes.normals.push_back(transformedNormal);
            }

            if (mesh->HasTextureCoords(0))
            {
                const auto& t = mesh->mTextureCoords[0][i];
                const Vector2f texCoord(t.x, -t.y);
                triangleMeshes.textureCoordinates.push_back(texCoord);
            }
        }
        bool nonTriangulatedFaces = false;
        nbFaces += mesh->mNumFaces;
        triangleMeshes.indices.reserve(nbFaces);
        const size_t offset = indexOffsets[mesh->mMaterialIndex];
        for (size_t f = 0; f < mesh->mNumFaces; ++f)
        {
            if (mesh->mFaces[f].mNumIndices == 3)
            {
                triangleMeshes.indices.push_back(
                    Vector3ui(offset + mesh->mFaces[f].mIndices[0],
                              offset + mesh->mFaces[f].mIndices[1],
                              offset + mesh->mFaces[f].mIndices[2]));
            }
            else
                nonTriangulatedFaces = true;
        }
        if (nonTriangulatedFaces)
            BRAYNS_DEBUG
                << "Some faces are not triangulated and have been removed"
                << std::endl;
        indexOffsets[mesh->mMaterialIndex] += mesh->mNumVertices;
    }

    BRAYNS_DEBUG << "Loaded " << nbVertices << " vertices and " << nbFaces
                 << " faces" << std::endl;
}

size_t MeshLoader::_getQuality() const
{
    switch (_geometryParameters.getGeometryQuality())
    {
    case GeometryQuality::low:
    case GeometryQuality::medium:
        return aiProcessPreset_TargetRealtime_Fast;
    case GeometryQuality::high:
    default:
        return aiProcess_GenSmoothNormals | aiProcess_Triangulate;
    }
}


std::string MeshLoader::getMeshFilenameFromGID(const uint64_t gid)
{
    const auto meshedMorphologiesFolder =
        _geometryParameters.getCircuitMeshFolder();
    auto meshFilenamePattern =
        _geometryParameters.getCircuitMeshFilenamePattern();
    const std::string gidAsString = std::to_string(gid);
    const std::string GID = "{gid}";
    if (!meshFilenamePattern.empty())
        meshFilenamePattern.replace(meshFilenamePattern.find(GID), GID.length(),
                                    gidAsString);
    else
        meshFilenamePattern = gidAsString;
    return meshedMorphologiesFolder + "/" + meshFilenamePattern;
}
#else
namespace
{
const std::string NO_ASSIMP_MESSAGE =
    "The assimp library is required to load meshes";
}

bool MeshLoader::importMeshFromFile(const std::string&, Scene&, const Matrix4f&,
                                    const size_t)
{
    BRAYNS_ERROR << NO_ASSIMP_MESSAGE << std::endl;
    return false;
}

bool MeshLoader::exportMeshToFile(const std::string&, Scene&) const
{
    BRAYNS_ERROR << NO_ASSIMP_MESSAGE << std::endl;
    return false;
}

std::string MeshLoader::getMeshFilenameFromGID(const uint64_t)
{
    BRAYNS_ERROR << NO_ASSIMP_MESSAGE << std::endl;
    return "";
}
#endif
}
