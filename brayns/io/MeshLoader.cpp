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
#ifdef BRAYNS_USE_ASSIMP
class ProgressWatcher : public Assimp::ProgressHandler
{
public:
    ProgressWatcher(const LoaderProgress& callback, const std::string& filename)
        : _callback(callback)

    {
        _msg << "Loading " << shortenString(filename) << " ..." << std::endl;
    }

    bool Update(const float percentage) final
    {
        constexpr float TOTAL_PROGRESS = 100.f;
        constexpr float LOADING_FRACTION = 50.f;
        _callback.updateProgress(_msg.str(), (percentage * LOADING_FRACTION) /
                                                 TOTAL_PROGRESS);
        return true;
    }

private:
    const LoaderProgress& _callback;
    std::function<void()> _cancelCheck;
    std::stringstream _msg;
};
#endif

MeshLoader::MeshLoader(Scene& scene,
                       const GeometryParameters& geometryParameters)
    : Loader(scene)
    , _geometryParameters(geometryParameters)
{
}

bool MeshLoader::isSupported(const std::string& filename BRAYNS_UNUSED,
                             const std::string& extension) const
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
    return types.find(extension) != types.end();
}

#ifdef BRAYNS_USE_ASSIMP
ModelDescriptorPtr MeshLoader::importFromFile(
    const std::string& fileName, const LoaderProgress& callback,
    const size_t index, const size_t defaultMaterialId) const
{
    auto model = _scene.createModel();
    importMesh(fileName, callback, *model, index, {}, defaultMaterialId);

    Transformation transformation;
    transformation.setRotationCenter(model->getBounds().getCenter());

    auto modelDescriptor =
        std::make_shared<ModelDescriptor>(std::move(model), fileName);
    modelDescriptor->setTransformation(transformation);
    return modelDescriptor;
}

ModelDescriptorPtr MeshLoader::importFromBlob(
    Blob&& blob, const LoaderProgress& callback, const size_t index,
    const size_t defaultMaterialId) const
{
    Assimp::Importer importer;
    importer.SetProgressHandler(new ProgressWatcher(callback, blob.name));

    const aiScene* aiScene =
        importer.ReadFileFromMemory(blob.data.data(), blob.data.size(),
                                    _getQuality(), blob.type.c_str());

    if (!aiScene)
        throw std::runtime_error(importer.GetErrorString());

    if (!aiScene->HasMeshes())
        throw std::runtime_error("No meshes found");

    auto model = _scene.createModel();
    _postLoad(aiScene, *model, index, {}, defaultMaterialId);

    Transformation transformation;
    transformation.setRotationCenter(model->getBounds().getCenter());

    auto modelDescriptor =
        std::make_shared<ModelDescriptor>(std::move(model), blob.name);
    modelDescriptor->setTransformation(transformation);
    return modelDescriptor;
}

void MeshLoader::_createMaterials(Model& model, const aiScene* aiScene,
                                  const std::string& folder) const
{
    BRAYNS_DEBUG << "Loading " << aiScene->mNumMaterials << " materials"
                 << std::endl;

    for (size_t m = 0; m < aiScene->mNumMaterials; ++m)
    {
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
        if (aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, value4f) == AI_SUCCESS)
            material->setDiffuseColor({value4f.r, value4f.g, value4f.b});

        value1f = 0.f;
        if (aimaterial->Get(AI_MATKEY_OPACITY, value1f) != AI_SUCCESS)
            material->setOpacity(value4f.a);
        else
            material->setOpacity(value1f);

        value1f = 0.f;
        if (aimaterial->Get(AI_MATKEY_REFLECTIVITY, value1f) == AI_SUCCESS)
            material->setReflectionIndex(value1f);

        value4f = aiColor4D(0.f);
        if (aimaterial->Get(AI_MATKEY_COLOR_SPECULAR, value4f) == AI_SUCCESS)
            material->setSpecularColor({value4f.r, value4f.g, value4f.b});

        value1f = 0.f;
        if (aimaterial->Get(AI_MATKEY_SHININESS, value1f) == AI_SUCCESS)
        {
            if (value1f == 0.f)
                material->setSpecularColor({0.f, 0.f, 0.f});
            material->setSpecularExponent(value1f);
        }

        value4f = aiColor4D(0.f);
        if (aimaterial->Get(AI_MATKEY_COLOR_EMISSIVE, value4f) == AI_SUCCESS)
            material->setEmission(value4f.r);

        value1f = 0.f;
        if (aimaterial->Get(AI_MATKEY_REFRACTI, value1f) == AI_SUCCESS)
            if (value1f != 0.f)
                material->setRefractionIndex(value1f);
    }
}

void MeshLoader::_postLoad(const aiScene* aiScene, Model& model,
                           const size_t index, const Matrix4f& transformation,
                           const size_t defaultMaterialId,
                           const std::string& folder) const
{
    const size_t materialId =
        _geometryParameters.getColorScheme() == ColorScheme::neuron_by_id
            ? index
            : defaultMaterialId;

    if (materialId == NO_MATERIAL)
        _createMaterials(model, aiScene, folder);

    size_t nbVertices = 0;
    size_t nbFaces = 0;
    std::map<size_t, size_t> indexOffsets;

    const auto trfm = aiScene->mRootNode->mTransformation;
    Matrix4f matrix;
    matrix.setRow(0, {trfm.a1, trfm.a2, trfm.a3, trfm.a4});
    matrix.setRow(1, {trfm.b1, trfm.b2, trfm.b3, trfm.b4});
    matrix.setRow(2, {trfm.c1, trfm.c2, trfm.c3, trfm.c4});
    matrix.setRow(3, {trfm.d1, trfm.d2, trfm.d3, trfm.d4});
    matrix = matrix * transformation;

    for (size_t m = 0; m < aiScene->mNumMeshes; ++m)
    {
        auto mesh = aiScene->mMeshes[m];
        auto id =
            (materialId != NO_MATERIAL ? materialId : mesh->mMaterialIndex);
        auto& triangleMeshes = model.getTrianglesMeshes()[id];

        nbVertices += mesh->mNumVertices;
        triangleMeshes.vertices.reserve(nbVertices);
        if (mesh->HasNormals())
            triangleMeshes.normals.reserve(nbVertices);
        if (mesh->HasTextureCoords(0))
            triangleMeshes.textureCoordinates.reserve(nbVertices);
        if (mesh->HasVertexColors(0))
            triangleMeshes.colors.reserve(nbVertices);
        for (size_t i = 0; i < mesh->mNumVertices; ++i)
        {
            const auto& v = mesh->mVertices[i];
            const Vector3f transformedVertex =
                matrix * Vector4f(v.x, v.y, v.z, 1.f);
            triangleMeshes.vertices.push_back(transformedVertex);
            if (mesh->HasNormals())
            {
                const auto& n = mesh->mNormals[i];
                const Vector4f normal = matrix * Vector4f(n.x, n.y, n.z, 0.f);
                const Vector3f transformedNormal = {normal.x(), normal.y(),
                                                    normal.z()};
                triangleMeshes.normals.push_back(transformedNormal);
            }

            if (mesh->HasTextureCoords(0))
            {
                const auto& t = mesh->mTextureCoords[0][i];
                triangleMeshes.textureCoordinates.push_back({t.x, -t.y});
            }

            if (mesh->HasVertexColors(0))
            {
                const auto& c = mesh->mColors[0][i];
                triangleMeshes.colors.push_back({c.r, c.g, c.b, c.a});
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

std::string MeshLoader::getMeshFilenameFromGID(const uint64_t gid) const
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

void MeshLoader::importMesh(const std::string& fileName,
                            const LoaderProgress& callback, Model& model,
                            const size_t index,
                            const vmml::Matrix4f& transformation,
                            const size_t defaultMaterialId) const
{
    const boost::filesystem::path file = fileName;
    Assimp::Importer importer;
    importer.SetProgressHandler(new ProgressWatcher(callback, fileName));
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

    _postLoad(aiScene, model, index, transformation, defaultMaterialId,
              filepath.parent_path().string());
}
#else
const std::runtime_error NO_ASSIMP(
    "The assimp library is required to load meshes");

ModelDescriptorPtr MeshLoader::importFromFile(const std::string&, const size_t,
                                              const size_t)
{
    throw NO_ASSIMP;
}

ModelDescriptorPtr MeshLoader::importFromBlob(Blob&&, const size_t,
                                              const size_t)
{
    throw NO_ASSIMP;
}

void MeshLoader::_createMaterials(Model&, const aiScene*, const std::string&)
{
    throw NO_ASSIMP;
}

void MeshLoader::_postLoad(const aiScene*, Model&, const size_t,
                           const Matrix4f&, size_t, const std::string&)
{
    throw NO_ASSIMP;
}

size_t MeshLoader::_getQuality() const
{
    throw NO_ASSIMP;
}
#endif
} // namespace brayns
