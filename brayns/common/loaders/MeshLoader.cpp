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

namespace brayns
{

MeshLoader::MeshLoader()
{
}

bool MeshLoader::importMeshFromFile(
        const std::string& filename,
        const float scale,
        TrianglesCollection& triangles,
        MaterialsCollection& materials,
        box3f& bounds,
        const int defaultMaterial)
{
    Assimp::Importer importer;
    const aiScene *scene = NULL;
    scene = importer.ReadFile( filename.c_str() , aiProcessPreset_TargetRealtime_MaxQuality);

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

    createMaterials_( scene, materials, defaultMaterial );

    size_t nbVertices = 0;
    size_t nbFaces = 0;
    for (size_t m=0; m<scene->mNumMeshes; ++m)
    {
        aiMesh* mesh = scene->mMeshes[m];
        size_t index = (defaultMaterial == NO_MATERIAL) ?
                    mesh->mMaterialIndex : defaultMaterial;

        nbVertices += mesh->mNumVertices;
        for (size_t i=0; i<mesh->mNumVertices; ++i)
        {
            aiVector3D v = mesh->mVertices[i];
            ospray::vec3fa vertex = {v.x*scale, v.y*scale, -v.z*scale};
            triangles[index].vertex.push_back(vertex);
            bounds.extend(vertex);

            if( mesh->mNormals )
            {
                v = mesh->mNormals[i];
                ospray::vec3fa normal = {v.x, v.y, -v.z};
                triangles[index].normal.push_back(normal);
            }
        }
        nbFaces += mesh->mNumFaces;
        for (size_t f=0; f<mesh->mNumFaces; ++f)
        {
            if( mesh->mFaces[f].mNumIndices == 3 )
            {
                ospray::vec3i ind = ospray::vec3i(
                            meshIndex_[index]+mesh->mFaces[f].mIndices[0],
                        meshIndex_[index]+mesh->mFaces[f].mIndices[1],
                        meshIndex_[index]+mesh->mFaces[f].mIndices[2]);
                triangles[index].index.push_back(ind);
            }
            else {
                BRAYNS_ERROR << "Face " << f <<
                                " is not triangulated" << std::endl;
            }
        }

        if( meshIndex_.find(index) == meshIndex_.end() )
            meshIndex_[index] = 0;

        meshIndex_[index] += mesh->mNumVertices;
    }

    BRAYNS_INFO << "Loaded " << nbVertices << " vertices and "
                << nbFaces << " faces" << std::endl;

    return true;
}

#ifdef __APPLE__
bool MeshLoader::exportMeshToFile(
        const std::string& filename,
        TrianglesCollection& triangles,
        MaterialsCollection& materials ) const
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
            const ospray::vec3fa& vertex = triangles[i].vertex[t]*1000.0;
            mesh.mVertices[t] = aiVector3D(vertex.x, vertex.y, vertex.z);
            const ospray::vec3fa& normal = triangles[i].normal[t];
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

void MeshLoader::createMaterials_(
        const aiScene *scene,
        MaterialsCollection& materials,
        const int defaultMaterial )
{
    if( defaultMaterial == NO_MATERIAL)
    {
        for (size_t m=0; m<scene->mNumMaterials; ++m)
        {
            aiMaterial* mat = scene->mMaterials[m];
            aiColor3D value3f(0.f,0.f,0.f);
            float value1f;
            mat->Get(AI_MATKEY_COLOR_DIFFUSE,value3f);
            ospSet3f(materials[m], "kd", value3f.r, value3f.g, value3f.b);

            value3f = aiColor3D(0.f,0.f,0.f);
            mat->Get(AI_MATKEY_COLOR_SPECULAR,value3f);
            ospSet3f(materials[m], "ks", value3f.r/10.f, value3f.g/10.f, value3f.b/10.f);

            value1f = 0.f;
            mat->Get(AI_MATKEY_SHININESS,value1f);
            ospSet1f(materials[m], "ns", value1f);

            value3f = aiColor3D(0.f,0.f,0.f);
            mat->Get(AI_MATKEY_COLOR_EMISSIVE,value3f);
            ospSet1f(materials[m], "a", value3f.r);

            value1f = 0.f;
            mat->Get(AI_MATKEY_REFLECTIVITY, value1f);
            ospSet1f(materials[m], "d", -value1f);

            value1f = 1.f;
            mat->Get(AI_MATKEY_OPACITY,value1f);
            ospSet1f(materials[m], "d", value1f);

            value1f = 0.9f;
            mat->Get(AI_MATKEY_REFRACTI,value1f);
            ospSet1f(materials[m], "r", value1f);

            ospCommit(materials[m]);
        }
    }

}

}
