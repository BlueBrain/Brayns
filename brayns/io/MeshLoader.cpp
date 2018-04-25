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

#if (BRAYNS_USE_ASSIMP)
#include <assimp/Exporter.hpp>
#include <assimp/IOSystem.hpp> // must come before Exporter.hpp
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <boost/filesystem.hpp>
#include <brayns/common/log.h>
#include <fstream>
#endif

#include <brayns/common/geometry/Model.h>
#include <brayns/common/scene/Scene.h>

namespace brayns
{
MeshLoader::MeshLoader(const GeometryParameters& geometryParameters)
    : _geometryParameters(geometryParameters)
{
}

void MeshLoader::clear()
{
}

#if (BRAYNS_USE_ASSIMP)
bool MeshLoader::importMeshFromFile(const std::string& fileName,
                                    const std::string& meshName,
                                    const size_t materialId, Model& model,
                                    const Matrix4f& transformation)
{
    const boost::filesystem::path file = fileName;
    Assimp::Importer importer;
    if (!importer.IsExtensionSupported(file.extension().c_str()))
    {
        BRAYNS_DEBUG << "File extension " << file.extension()
                     << " is not supported" << std::endl;
        return false;
    }

    size_t quality;
    switch (_geometryParameters.getGeometryQuality())
    {
    case GeometryQuality::low:
    case GeometryQuality::medium:
        quality = aiProcessPreset_TargetRealtime_Fast;
        break;
    case GeometryQuality::high:
    default:
        quality = aiProcess_GenSmoothNormals | aiProcess_Triangulate;
        break;
    }

    std::ifstream meshFile(fileName, std::ios::in);
    if (!meshFile.good())
    {
        BRAYNS_DEBUG << "Could not open file " << fileName << std::endl;
        return false;
    }
    meshFile.close();

    const aiScene* aiScene = nullptr;
    aiScene = importer.ReadFile(fileName.c_str(), quality);

    if (!aiScene)
    {
        BRAYNS_DEBUG << "Error parsing " << fileName.c_str() << ": "
                     << importer.GetErrorString() << std::endl;
        return false;
    }

    if (!aiScene->HasMeshes())
    {
        BRAYNS_DEBUG << "Error Finding Model In file. "
                     << "Did you export an empty scene?" << std::endl;
        return false;
    }

    boost::filesystem::path filepath = fileName;

    if (materialId == NO_MATERIAL)
        _createMaterials(meshName, model, aiScene,
                         filepath.parent_path().string());

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
    return true;
}

void MeshLoader::_createMaterials(const std::string& meshName, Model& model,
                                  const aiScene* aiScene,
                                  const std::string& folder BRAYNS_UNUSED)
{
    BRAYNS_DEBUG << "Loading " << aiScene->mNumMaterials << " materials"
                 << std::endl;

    for (size_t m = 0; m < aiScene->mNumMaterials; ++m)
    {
        aiMaterial* aimaterial = aiScene->mMaterials[m];

        aiString valueString;
        aimaterial->Get(AI_MATKEY_NAME, valueString);
        std::string name{valueString.C_Str()};
        auto material = model.createMaterial(m, meshName + '_' + name);

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

        aiColor3D value3f(0.f, 0.f, 0.f);
        float value1f;
        aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, value3f);
        material->setDiffuseColor(Vector3f(value3f.r, value3f.g, value3f.b));

        value1f = 0.f;
        aimaterial->Get(AI_MATKEY_REFLECTIVITY, value1f);
        // material.setReflectionIndex(value1f);
        material->setReflectionIndex(value1f);

        value3f = aiColor3D(0.f, 0.f, 0.f);
        aimaterial->Get(AI_MATKEY_COLOR_SPECULAR, value3f);
        material->setSpecularColor(Vector3f(value3f.r, value3f.g, value3f.b));

        value1f = 0.f;
        aimaterial->Get(AI_MATKEY_SHININESS, value1f);
        material->setSpecularExponent(fabs(value1f) < 0.01f ? 100.f : value1f);

        value3f = aiColor3D(0.f, 0.f, 0.f);
        aimaterial->Get(AI_MATKEY_COLOR_EMISSIVE, value3f);
        material->setEmission(value3f.r);

        value1f = 0.f;
        aimaterial->Get(AI_MATKEY_OPACITY, value1f);
        material->setOpacity(fabs(value1f) < 0.01f ? 1.f : value1f);

        value1f = 0.f;
        aimaterial->Get(AI_MATKEY_REFRACTI, value1f);
        material->setRefractionIndex(fabs(value1f - 1.f) < 0.01f ? 1.0f
                                                                 : value1f);
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

bool MeshLoader::importMeshFromFile(const std::string&, GeometryGroup&,
                                    const Matrix4f&, const size_t)
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
