/* Copyright (c) 2011-2015, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/postprocess.h>

#include <brayns/common/log.h>
#include <boost/filesystem.hpp>

namespace brayns
{

MeshLoader::MeshLoader()
{
}

bool MeshLoader::importMeshFromFile(
        const std::string& filename,
        const float scale,
        TrianglesMeshMap& triangles,
        Materials& materials,
        const int defaultMaterial,
        const MeshQuality meshQuality,
        Boxf& bounds)
{
    size_t quality;
    switch(meshQuality)
    {
        case MQ_QUALITY:
            quality = aiProcessPreset_TargetRealtime_Quality;
            break;
        case MQ_MAX_QUALITY:
            quality = aiProcessPreset_TargetRealtime_MaxQuality;
            break;
        default:
            quality = aiProcessPreset_TargetRealtime_Fast;
            break;
    }

    Assimp::Importer importer;
    const aiScene *scene = nullptr;
    scene = importer.ReadFile( filename.c_str() , quality);

    if (!scene)
    {
        BRAYNS_ERROR << "Error parsing " << filename.c_str() << ": " <<
                        importer.GetErrorString() << std::endl;
        return false;
    }

    if (!scene->HasMeshes())
    {
        BRAYNS_ERROR << "Error Finding Model In file. " <<
                        "Did you export an empty scene?" << std::endl;
        return false;
    }

    boost::filesystem::path filepath = filename;
    _createMaterials(scene, filepath.parent_path().string(), materials, defaultMaterial);

    size_t nbVertices = 0;
    size_t nbFaces = 0;
    for(size_t m=0; m<scene->mNumMeshes; ++m)
    {
        aiMesh* mesh = scene->mMeshes[m];
        size_t index = (defaultMaterial == NO_MATERIAL) ?
                    mesh->mMaterialIndex : defaultMaterial;

        nbVertices += mesh->mNumVertices;
        for(size_t i=0; i<mesh->mNumVertices; ++i)
        {
            aiVector3D v = mesh->mVertices[i];
            Vector3f vertex = {v.x*scale, v.y*scale, v.z*scale};
            triangles[index].getVertices().push_back(vertex);
            bounds.merge(vertex);

            if(mesh->HasNormals())
            {
                v = mesh->mNormals[i];
                Vector3f normal = {v.x, v.y, v.z};
                triangles[index].getNormals().push_back(normal);
            }

            if(mesh->HasTextureCoords(0))
            {
                v = mesh->mTextureCoords[0][i];
                Vector2f texCoord( v.x, -v.y );
                triangles[index].getTextureCoordinates().push_back(texCoord);
            }
        }
        nbFaces += mesh->mNumFaces;
        for(size_t f=0; f<mesh->mNumFaces; ++f)
        {
            if(mesh->mFaces[f].mNumIndices==3)
            {
                Vector3i ind = Vector3i(
                    _meshIndex[index]+mesh->mFaces[f].mIndices[0],
                    _meshIndex[index]+mesh->mFaces[f].mIndices[1],
                    _meshIndex[index]+mesh->mFaces[f].mIndices[2]);
                triangles[index].getIndices().push_back(ind);
            }
            else {
                BRAYNS_ERROR << "Face " << f << " is not triangulated" << std::endl;
            }
        }

        if(_meshIndex.find(index) == _meshIndex.end())
           _meshIndex[index] = 0;

        _meshIndex[index] += mesh->mNumVertices;
    }

    BRAYNS_INFO << "Loaded " << nbVertices << " vertices and "
                << nbFaces << " faces" << std::endl;

    return true;
}

#ifdef __APPLE__
bool MeshLoader::exportMeshToFile(
        const std::string& filename,
        TrianglesMeshMap& triangles,
        Materials& materials ) const
{
    // Save to OBJ
    size_t nbMaterials = materials.size();
    aiScene scene;
    scene.mMaterials = new aiMaterial*[nbMaterials];
    scene.mNumMaterials = nbMaterials;

    aiNode* rootNode = new aiNode();
    rootNode->mName = "brayns";
    scene.mRootNode = rootNode;
    rootNode->mNumMeshes = nbMaterials;
    rootNode->mMeshes = new unsigned int[rootNode->mNumMeshes];

    for( size_t i=0; i<rootNode->mNumMeshes; ++i)
    {
        rootNode->mMeshes[i] = i;

        // Materials
        aiMaterial* material = new aiMaterial();
        const aiVector3D c(rand()%255/255,rand()%255/255,rand()%255/255);
        material->AddProperty(&c, 1, AI_MATKEY_COLOR_DIFFUSE);
        material->AddProperty(&c, 1, AI_MATKEY_COLOR_SPECULAR );
        scene.mMaterials[i] = material;
    }

    scene.mNumMeshes = nbMaterials;
    scene.mMeshes = new aiMesh*[materials.size()];
    size_t numVertex = 0;
    size_t numFace = 0;
    for( size_t i=0; i<scene.mNumMeshes; ++i)
    {
        aiMesh mesh;
        mesh.mNumVertices = triangles[i].vertex.size();
        mesh.mNumFaces = triangles[i].index.size();
        mesh.mMaterialIndex = i;
        mesh.mVertices = new aiVector3D[mesh.mNumVertices];
        mesh.mFaces = new aiFace[mesh.mNumFaces];
        mesh.mNormals = new aiVector3D[mesh.mNumVertices];

        for( size_t t(0); t<triangles[i].index.size(); ++t)
        {
            aiFace face;
            face.mNumIndices = 3;
            face.mIndices = new unsigned int[face.mNumIndices];
            face.mIndices[0] = triangles[i].index[t].x;
            face.mIndices[1] = triangles[i].index[t].y;
            face.mIndices[2] = triangles[i].index[t].z;
            mesh.mFaces[t] = face;
            ++numFace;
        }

        // vertices
        for( size_t t=0; t<triangles[i].vertex.size(); ++t)
        {
            const Vector3f& vertex = triangles[i].vertex[t]*1000.0;
            mesh.mVertices[t] = aiVector3D(vertex.x, vertex.y, vertex.z);
            const Vector3f& normal = triangles[i].normal[t];
            mesh.mNormals[t] = aiVector3D(normal.x, normal.y, normal.z);
            ++numVertex;
        }
        scene.mMeshes[i] = &mesh;
    }
    Assimp::Exporter exporter;
    exporter.Export( &scene, "obj", filename, aiProcess_MakeLeftHanded );

    BRAYNS_INFO << "Exported OBJ model to " << filename << std::endl;
    return true;
}
#endif

void MeshLoader::_createMaterials(
        const aiScene *scene,
        const std::string& folder,
        Materials& materials,
        const int defaultMaterial )
{
    BRAYNS_INFO << "Loading materials" << std::endl;
    if(defaultMaterial==NO_MATERIAL)
    {
        for(size_t m=0; m<scene->mNumMaterials; ++m)
        {
            aiMaterial* material = scene->mMaterials[m];

            struct TextureTypeMapping
            {
                aiTextureType aiType;
                TextureType type;
            };

            const size_t NB_TEXTURE_TYPES = 6;
            TextureTypeMapping textureTypeMapping[NB_TEXTURE_TYPES] =
            {
                {aiTextureType_DIFFUSE, TT_DIFFUSE},
                {aiTextureType_NORMALS, TT_NORMALS},
                {aiTextureType_SPECULAR, TT_SPECULAR},
                {aiTextureType_EMISSIVE, TT_EMISSIVE},
                {aiTextureType_OPACITY, TT_OPACITY},
                {aiTextureType_REFLECTION, TT_REFLECTION}
            };

            for(size_t textureType=0; textureType<NB_TEXTURE_TYPES; ++textureType)
            {
                if(material->GetTextureCount(textureTypeMapping[textureType].aiType)>0)
                {
                    aiString path;
                    if(material->GetTexture(textureTypeMapping[textureType].aiType,
                        0, &path, nullptr, nullptr, nullptr, nullptr, nullptr ) == AI_SUCCESS)
                    {
                        materials[m]->getTextures()[textureTypeMapping[textureType].type] =
                             folder + "/" + path.data;
                    }
                }
            }

            aiColor3D value3f(0.f,0.f,0.f);
            float value1f;
            material->Get(AI_MATKEY_COLOR_DIFFUSE,value3f);
            materials[m]->setColor(Vector3f(value3f.r, value3f.g, value3f.b));

            value3f = aiColor3D(0.f,0.f,0.f);
            material->Get(AI_MATKEY_COLOR_SPECULAR,value3f);
            materials[m]->setSpecularColor(Vector3f(value3f.r, value3f.g, value3f.b));

            value1f = 0.f;
            material->Get(AI_MATKEY_SHININESS,value1f);
            materials[m]->setSpecularExponent(value1f);

            value3f = aiColor3D(0.f,0.f,0.f);
            material->Get(AI_MATKEY_COLOR_EMISSIVE,value3f);
            materials[m]->setEmission(value3f.r);

            value1f = 0.f;
            material->Get(AI_MATKEY_REFLECTIVITY, value1f);
            materials[m]->setReflectionIndex(value1f);

            value1f = 1.f;
            material->Get(AI_MATKEY_OPACITY,value1f);
            materials[m]->setOpacity(value1f);

            value1f = 0.9f;
            material->Get(AI_MATKEY_REFRACTI,value1f);
            materials[m]->setRefractionIndex(value1f);
        }
    }
}

}
