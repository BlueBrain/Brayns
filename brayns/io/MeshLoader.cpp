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

#include <brayns/common/scene/Scene.h>

namespace brayns
{
MeshLoader::MeshLoader(const GeometryParameters& geometryParameters)
    : _geometryParameters(geometryParameters)
{
}

void MeshLoader::clear()
{
    _meshIndex.clear();
}

#if (BRAYNS_USE_ASSIMP)
bool MeshLoader::importMeshFromFile(const std::string& filename, Scene& scene,
                                    const Matrix4f& transformation,
                                    const size_t defaultMaterial)
{
    const boost::filesystem::path file = filename;
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

    std::ifstream meshFile(filename, std::ios::in);
    if (!meshFile.good())
    {
        BRAYNS_DEBUG << "Could not open file " << filename << std::endl;
        return false;
    }
    meshFile.close();

    const aiScene* aiScene = nullptr;
    aiScene = importer.ReadFile(filename.c_str(), quality);

    if (!aiScene)
    {
        BRAYNS_DEBUG << "Error parsing " << filename.c_str() << ": "
                     << importer.GetErrorString() << std::endl;
        return false;
    }

    if (!aiScene->HasMeshes())
    {
        BRAYNS_DEBUG << "Error Finding Model In file. "
                     << "Did you export an empty scene?" << std::endl;
        return false;
    }

    boost::filesystem::path filepath = filename;
    if (defaultMaterial == NO_MATERIAL)
        _createMaterials(scene, aiScene, filepath.parent_path().string());

    size_t nbVertices = 0;
    size_t nbFaces = 0;
    auto& triangleMeshes = scene.getTriangleMeshes();
    for (size_t m = 0; m < aiScene->mNumMeshes; ++m)
    {
        aiMesh* mesh = aiScene->mMeshes[m];
        const size_t materialId =
            (defaultMaterial == NO_MATERIAL
                 ? NB_SYSTEM_MATERIALS + mesh->mMaterialIndex
                 : defaultMaterial);

        auto& triangleMesh = triangleMeshes[materialId];

        nbVertices += mesh->mNumVertices;
        triangleMesh.vertices.reserve(nbVertices);
        if (mesh->HasNormals())
            triangleMesh.normals.reserve(nbVertices);
        if (mesh->HasTextureCoords(0))
            triangleMesh.textureCoordinates.reserve(nbVertices);
        for (size_t i = 0; i < mesh->mNumVertices; ++i)
        {
            const auto& v = mesh->mVertices[i];
            const Vector4f vertex =
                transformation * Vector4f(v.x, v.y, v.z, 1.f);
            const Vector3f transformedVertex = {vertex.x(), vertex.y(),
                                                vertex.z()};
            triangleMesh.vertices.push_back(transformedVertex);
            scene.getWorldBounds().merge(transformedVertex);
            if (mesh->HasNormals())
            {
                const auto& n = mesh->mNormals[i];
                const Vector4f normal =
                    transformation * Vector4f(n.x, n.y, n.z, 0.f);
                const Vector3f transformedNormal = {normal.x(), normal.y(),
                                                    normal.z()};
                triangleMesh.normals.push_back(transformedNormal);
            }

            if (mesh->HasTextureCoords(0))
            {
                const auto& t = mesh->mTextureCoords[0][i];
                const Vector2f texCoord(t.x, -t.y);
                triangleMesh.textureCoordinates.push_back(texCoord);
            }
        }
        bool nonTriangulatedFaces = false;
        nbFaces += mesh->mNumFaces;
        triangleMesh.indices.reserve(nbFaces);
        if (_meshIndex.find(materialId) == _meshIndex.end())
            _meshIndex[materialId] = 0;
        const auto meshIndex = _meshIndex[materialId];
        for (size_t f = 0; f < mesh->mNumFaces; ++f)
        {
            if (mesh->mFaces[f].mNumIndices == 3)
            {
                const Vector3ui ind =
                    Vector3ui(meshIndex + mesh->mFaces[f].mIndices[0],
                              meshIndex + mesh->mFaces[f].mIndices[1],
                              meshIndex + mesh->mFaces[f].mIndices[2]);
                triangleMesh.indices.push_back(ind);
            }
            else
                nonTriangulatedFaces = true;
        }
        if (nonTriangulatedFaces)
            BRAYNS_DEBUG
                << "Some faces are not triangulated and have been removed"
                << std::endl;

        _meshIndex[materialId] += mesh->mNumVertices;
    }

    BRAYNS_DEBUG << "Loaded " << nbVertices << " vertices and " << nbFaces
                 << " faces" << std::endl;

    return true;
}

bool MeshLoader::exportMeshToFile(const std::string& filename,
                                  Scene& scene) const
{
    // Save to OBJ
    size_t nbMaterials = scene.getMaterials().size();
    aiScene aiScene;
    aiScene.mMaterials = new aiMaterial*[nbMaterials];
    aiScene.mNumMaterials = nbMaterials;

    aiNode* rootNode = new aiNode();
    rootNode->mName = "brayns";
    aiScene.mRootNode = rootNode;
    rootNode->mNumMeshes = nbMaterials;
    rootNode->mMeshes = new unsigned int[rootNode->mNumMeshes];

    for (size_t i = 0; i < rootNode->mNumMeshes; ++i)
    {
        rootNode->mMeshes[i] = i;

        // Materials
        aiMaterial* material = new aiMaterial();
        const aiVector3D c(rand() % 255 / 255, rand() % 255 / 255,
                           rand() % 255 / 255);
        material->AddProperty(&c, 1, AI_MATKEY_COLOR_DIFFUSE);
        material->AddProperty(&c, 1, AI_MATKEY_COLOR_SPECULAR);
        aiScene.mMaterials[i] = material;
    }

    aiScene.mNumMeshes = nbMaterials;
    aiScene.mMeshes = new aiMesh*[scene.getMaterials().size()];
    size_t numVertex = 0;
    size_t numFace = 0;
    auto& triangleMeshes = scene.getTriangleMeshes();
    for (size_t meshIndex = 0; meshIndex < aiScene.mNumMeshes; ++meshIndex)
    {
        aiMesh mesh;
        mesh.mNumVertices = triangleMeshes[meshIndex].vertices.size();
        mesh.mNumFaces = triangleMeshes[meshIndex].indices.size();
        mesh.mMaterialIndex = meshIndex;
        mesh.mVertices = new aiVector3D[mesh.mNumVertices];
        mesh.mFaces = new aiFace[mesh.mNumFaces];
        mesh.mNormals = new aiVector3D[mesh.mNumVertices];

        for (size_t t = 0; t < triangleMeshes[meshIndex].indices.size(); ++t)
        {
            aiFace face;
            face.mNumIndices = 3;
            face.mIndices = new unsigned int[face.mNumIndices];
            face.mIndices[0] = triangleMeshes[meshIndex].indices[t].x();
            face.mIndices[1] = triangleMeshes[meshIndex].indices[t].y();
            face.mIndices[2] = triangleMeshes[meshIndex].indices[t].z();
            mesh.mFaces[t] = face;
            ++numFace;
        }

        for (size_t t = 0; t < triangleMeshes[meshIndex].vertices.size(); ++t)
        {
            const Vector3f& vertex = triangleMeshes[meshIndex].vertices[t];
            mesh.mVertices[t] = aiVector3D(vertex.x(), vertex.y(), vertex.z());
            const Vector3f& normal = triangleMeshes[meshIndex].normals[t];
            mesh.mNormals[t] = aiVector3D(normal.x(), normal.y(), normal.z());
            ++numVertex;
        }
        aiScene.mMeshes[meshIndex] = &mesh;
    }
    Assimp::Exporter exporter;
    exporter.Export(&aiScene, "obj", filename, aiProcess_MakeLeftHanded);

    BRAYNS_INFO << "Exported OBJ model to " << filename << std::endl;
    return true;
}

void MeshLoader::_createMaterials(Scene& scene, const aiScene* aiScene,
                                  const std::string& folder)
{
    BRAYNS_DEBUG << "Loading " << aiScene->mNumMaterials << " materials"
                 << std::endl;
    for (size_t m = 0; m < aiScene->mNumMaterials; ++m)
    {
        const size_t materialId = NB_SYSTEM_MATERIALS + m;
        aiMaterial* material = aiScene->mMaterials[m];
        auto& materials = scene.getMaterials();
        auto& mat = materials[materialId];

        struct TextureTypeMapping
        {
            aiTextureType aiType;
            TextureType type;
        };

        const size_t NB_TEXTURE_TYPES = 6;
        TextureTypeMapping textureTypeMapping[NB_TEXTURE_TYPES] = {
            {aiTextureType_DIFFUSE, TT_DIFFUSE},
            {aiTextureType_NORMALS, TT_NORMALS},
            {aiTextureType_SPECULAR, TT_SPECULAR},
            {aiTextureType_EMISSIVE, TT_EMISSIVE},
            {aiTextureType_OPACITY, TT_OPACITY},
            {aiTextureType_REFLECTION, TT_REFLECTION}};

        for (size_t textureType = 0; textureType < NB_TEXTURE_TYPES;
             ++textureType)
        {
            if (material->GetTextureCount(
                    textureTypeMapping[textureType].aiType) > 0)
            {
                aiString path;
                if (material->GetTexture(textureTypeMapping[textureType].aiType,
                                         0, &path, nullptr, nullptr, nullptr,
                                         nullptr, nullptr) == AI_SUCCESS)
                {
                    const std::string filename = folder + "/" + path.data;
                    BRAYNS_DEBUG << "Loading texture [" << materialId
                                 << "] :" << filename << std::endl;
                    mat.getTextures()[textureTypeMapping[textureType].type] =
                        filename;
                }
            }
        }

        aiColor3D value3f(0.f, 0.f, 0.f);
        float value1f;
        material->Get(AI_MATKEY_COLOR_DIFFUSE, value3f);
        mat.setColor(Vector3f(value3f.r, value3f.g, value3f.b));

        value1f = 0.f;
        material->Get(AI_MATKEY_REFLECTIVITY, value1f);
        mat.setReflectionIndex(value1f);

        value3f = aiColor3D(0.f, 0.f, 0.f);
        material->Get(AI_MATKEY_COLOR_SPECULAR, value3f);
        mat.setSpecularColor(Vector3f(value3f.r, value3f.g, value3f.b));

        value1f = 0.f;
        material->Get(AI_MATKEY_SHININESS, value1f);
        mat.setSpecularExponent(fabs(value1f) < 0.01f ? 100.f : value1f);

        value3f = aiColor3D(0.f, 0.f, 0.f);
        material->Get(AI_MATKEY_COLOR_EMISSIVE, value3f);
        mat.setEmission(value3f.r);

        value1f = 0.f;
        material->Get(AI_MATKEY_OPACITY, value1f);
        mat.setOpacity(fabs(value1f) < 0.01f ? 1.f : value1f);

        value1f = 0.f;
        material->Get(AI_MATKEY_REFRACTI, value1f);
        mat.setRefractionIndex(fabs(value1f - 1.f) < 0.01f ? 1.0f : value1f);
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
