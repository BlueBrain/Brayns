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

#include "OptiXScene.h"
#include "OptiXUtils.h"

#include <brayns/common/light/DirectionalLight.h>
#include <brayns/common/light/PointLight.h>
#include <brayns/common/log.h>
#include <brayns/common/volume/VolumeHandler.h>
#include <brayns/io/TextureLoader.h>
#include <brayns/parameters/ParametersManager.h>

#include <plugins/engines/optix/cuda/braynsOptiXCudaPlugin_generated_Cones.cu.ptx.h>
#include <plugins/engines/optix/cuda/braynsOptiXCudaPlugin_generated_Cylinders.cu.ptx.h>
#include <plugins/engines/optix/cuda/braynsOptiXCudaPlugin_generated_Phong.cu.ptx.h>
#include <plugins/engines/optix/cuda/braynsOptiXCudaPlugin_generated_Spheres.cu.ptx.h>
#include <plugins/engines/optix/cuda/braynsOptiXCudaPlugin_generated_TrianglesMesh.cu.ptx.h>

#include <boost/algorithm/string/predicate.hpp> // ends_with

namespace
{
const std::string DEFAULT_ACCELERATION_STRUCTURE = "Trbvh";
char const* const CUDA_SPHERES = braynsOptiXCudaPlugin_generated_Spheres_cu_ptx;
char const* const CUDA_CYLINDERS =
    braynsOptiXCudaPlugin_generated_Cylinders_cu_ptx;
char const* const CUDA_CONES = braynsOptiXCudaPlugin_generated_Cones_cu_ptx;
char const* const CUDA_TRIANGLES_MESH =
    braynsOptiXCudaPlugin_generated_TrianglesMesh_cu_ptx;
const std::string CUDA_FUNCTION_BOUNDS = "bounds";
const std::string CUDA_FUNCTION_INTERSECTION = "intersect";
const std::string CUDA_FUNCTION_ROBUST_INTERSECTION = "robust_intersect";
char const* const CUDA_PHONG = braynsOptiXCudaPlugin_generated_Phong_cu_ptx;
}

namespace brayns
{
OptiXScene::OptiXScene(Renderers renderer, ParametersManager& parametersManager,
                       optix::Context& context)
    : Scene(renderer, parametersManager)
    , _context(context)
    , _lightBuffer(nullptr)
    , _accelerationStructure(DEFAULT_ACCELERATION_STRUCTURE)
    , _colorMapBuffer(nullptr)
    , _mesh(nullptr)
    , _verticesBuffer(nullptr)
    , _indicesBuffer(nullptr)
    , _normalsBuffer(nullptr)
    , _textureCoordsBuffer(nullptr)
    , _materialsBuffer(nullptr)
    , _volumeBuffer(nullptr)
{
    // Compile Kernels
    _phong_ch = _context->createProgramFromPTXString(CUDA_PHONG,
                                                     "closest_hit_radiance");
    _phong_ch_textured =
        _context->createProgramFromPTXString(CUDA_PHONG,
                                             "closest_hit_radiance_textured");
    _phong_ah =
        _context->createProgramFromPTXString(CUDA_PHONG, "any_hit_shadow");

    _spheresBoundsProgram =
        _context->createProgramFromPTXString(CUDA_SPHERES,
                                             CUDA_FUNCTION_BOUNDS);
    _spheresIntersectProgram =
        _context->createProgramFromPTXString(CUDA_SPHERES,
                                             CUDA_FUNCTION_INTERSECTION);

    _cylindersBoundsProgram =
        _context->createProgramFromPTXString(CUDA_CYLINDERS,
                                             CUDA_FUNCTION_BOUNDS);
    _cylindersIntersectProgram =
        _context->createProgramFromPTXString(CUDA_CYLINDERS,
                                             CUDA_FUNCTION_INTERSECTION);

    _conesBoundsProgram =
        _context->createProgramFromPTXString(CUDA_CONES, CUDA_FUNCTION_BOUNDS);
    _conesIntersectProgram =
        _context->createProgramFromPTXString(CUDA_CONES,
                                             CUDA_FUNCTION_INTERSECTION);

    _meshBoundsProgram =
        _context->createProgramFromPTXString(CUDA_TRIANGLES_MESH,
                                             CUDA_FUNCTION_BOUNDS);
    _meshIntersectProgram =
        _context->createProgramFromPTXString(CUDA_TRIANGLES_MESH,
                                             CUDA_FUNCTION_INTERSECTION);
}

OptiXScene::~OptiXScene()
{
    reset();
}

void OptiXScene::reset()
{
    Scene::reset();

    // Geometry
    _geometryInstances.clear();
    _geometryGroup = nullptr;

    // Volume
    _volumeBuffer = nullptr;

    // Color map
    _colorMapBuffer = nullptr;

    // Spheres
    _spheresBuffers.clear();
    _optixSpheres.clear();
    _serializedSpheresData.clear();
    _serializedSpheresDataSize.clear();
    _timestampSpheresIndices.clear();

    // Cylinders
    _cylindersBuffers.clear();
    _optixCylinders.clear();
    _serializedCylindersData.clear();
    _timestampCylindersIndices.clear();
    _serializedCylindersDataSize.clear();

    // Cones
    _conesBuffers.clear();
    _optixCones.clear();
    _serializedConesData.clear();
    _serializedConesDataSize.clear();
    _timestampConesIndices.clear();

    // Meshes
    _mesh = nullptr;
    _verticesBuffer = nullptr;
    _indicesBuffer = nullptr;
    _normalsBuffer = nullptr;
    _textureCoordsBuffer = nullptr;
    _materialsBuffer = nullptr;

    // Lights
    _lightBuffer = nullptr;

    // Textures
    _optixTextures.clear();
    _optixTextureSamplers.clear();
}

void OptiXScene::commit()
{
    if (_geometryGroup)
        _geometryGroup->destroy();
    _geometryGroup = _context->createGeometryGroup();
    _geometryGroup->setAcceleration(
        _context->createAcceleration(_accelerationStructure,
                                     _accelerationStructure));
    _geometryGroup->setChildCount(_geometryInstances.size());

    BRAYNS_INFO << "Adding " << _geometryInstances.size()
                << " geometry instances" << std::endl;
    for (size_t i = 0; i < _geometryInstances.size(); ++i)
        _geometryGroup->setChild(i, _geometryInstances[i]);

    BRAYNS_INFO << "Acceleration size: "
                << _geometryGroup->getAcceleration()->getDataSize()
                << std::endl;

    _context["top_object"]->set(_geometryGroup);
    _context["top_shadower"]->set(_geometryGroup);

    _context->validate();
}

void OptiXScene::buildGeometry()
{
    BRAYNS_INFO << "Building OptiX geometry" << std::endl;

    // Make sure lights and materials have been initialized before assigning
    // the geometry
    commitMaterials();
    commitLights();

    _geometryInstances.clear();

    BRAYNS_INFO << "----------------------------------------" << std::endl;
    BRAYNS_INFO << "Data information:" << std::endl;

    uint64_t paramGeomMemSize = serializeGeometry();
    uint64_t meshMemSize = _processMeshes();
    uint64_t totalGPUMemory = paramGeomMemSize + meshMemSize;

    BRAYNS_INFO << "Total GPU : " << totalGPUMemory << " bytes ("
                << totalGPUMemory / 1048576 << " MB)" << std::endl;
    BRAYNS_INFO << "----------------------------------------" << std::endl;

    // Geometry hierarchy
    commit();
}

void OptiXScene::commitLights()
{
    _optixLights.clear();
    if (_lights.size() == 0)
    {
        BRAYNS_ERROR << "No lights are currently defined" << std::endl;
        return;
    }

    for (auto light : _lights)
    {
        PointLight* pointLight = dynamic_cast<PointLight*>(light.get());
        if (pointLight != 0)
        {
            const Vector3f& position = pointLight->getPosition();
            const Vector3f& color = pointLight->getColor();
            BasicLight optixLight = {{position.x(), position.y(), position.z()},
                                     {color.x(), color.y(), color.z()},
                                     1, // Casts shadows
                                     LT_POINT};
            _optixLights.push_back(optixLight);
        }
        else
        {
            DirectionalLight* directionalLight =
                dynamic_cast<DirectionalLight*>(light.get());
            if (directionalLight)
            {
                const Vector3f& direction = directionalLight->getDirection();
                const Vector3f& color = directionalLight->getColor();
                BasicLight optixLight = {{direction.x(), direction.y(),
                                          direction.z()},
                                         {color.x(), color.y(), color.z()},
                                         1, // Casts shadows
                                         LT_DIRECTIONAL};
                _optixLights.push_back(optixLight);
            }
        }
    }

    if (!_lightBuffer)
    {
        _lightBuffer = _context->createBuffer(RT_BUFFER_INPUT);
        _lightBuffer->setFormat(RT_FORMAT_USER);
        _lightBuffer->setElementSize(sizeof(BasicLight));
        _lightBuffer->setSize(_optixLights.size());
        _context["lights"]->set(_lightBuffer);
    }

    memcpy(_lightBuffer->map(), &_optixLights[0],
           _optixLights.size() * sizeof(BasicLight));
    _lightBuffer->unmap();
}

uint64_t OptiXScene::_serializeSpheres()
{
    // Load geometry to GPU
    size_t totalNbSpheres = 0;
    for (size_t materialId = 0; materialId < _materials.size(); ++materialId)
    {
        _timestampSpheresIndices[materialId] = 0;
        if (_spheres.find(materialId) != _spheres.end())
        {
            for (const auto& sphere : _spheres[materialId])
            {
                sphere->serializeData(_serializedSpheresData[materialId]);
                ++_serializedSpheresDataSize[materialId];
                ++_timestampSpheresIndices[materialId];
            }
            totalNbSpheres += _timestampSpheresIndices[materialId];
        }

        // Create spheres geometry
        if (_timestampSpheresIndices[materialId] != 0)
        {
            _optixSpheres[materialId] = _context->createGeometry();
            _optixSpheres[materialId]->setPrimitiveCount(
                _timestampSpheresIndices[materialId]);
            _optixSpheres[materialId]->setBoundingBoxProgram(
                _spheresBoundsProgram);
            _optixSpheres[materialId]->setIntersectionProgram(
                _spheresIntersectProgram);
            uint64_t size = _timestampSpheresIndices[materialId] *
                            Sphere::getSerializationSize();
            if (!_spheresBuffers[materialId])
                _spheresBuffers[materialId] =
                    _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT,
                                           size);
            memcpy(_spheresBuffers[materialId]->map(),
                   &_serializedSpheresData[materialId][0],
                   size * sizeof(float));
            _spheresBuffers[materialId]->unmap();
            _optixSpheres[materialId]["spheres"]->setBuffer(
                _spheresBuffers[materialId]);
            _geometryInstances.push_back(_context->createGeometryInstance(
                _optixSpheres[materialId], &_optixMaterials[materialId],
                &_optixMaterials[materialId] + 1));
            _serializedSpheresData[materialId].clear();
        }
    }

    const uint64_t memSize = _getBvhSize(
        totalNbSpheres * Sphere::getSerializationSize() * sizeof(float));
    BRAYNS_DEBUG << "- Spheres   : " << totalNbSpheres << " [" << memSize
                 << " bytes]" << std::endl;
    return memSize;
}

uint64_t OptiXScene::_serializeCylinders()
{
    size_t totalNbCylinders = 0;
    for (size_t materialId = 0; materialId < _materials.size(); ++materialId)
    {
        _timestampCylindersIndices[materialId] = 0;

        if (_cylinders.find(materialId) != _cylinders.end())
        {
            for (const auto& cylinder : _cylinders[materialId])
            {
                cylinder->serializeData(_serializedCylindersData[materialId]);
                ++_serializedCylindersDataSize[materialId];
                ++_timestampCylindersIndices[materialId];
            }
            totalNbCylinders += _timestampCylindersIndices[materialId];
        }

        // Create cylinders geometry
        if (_timestampCylindersIndices[materialId] != 0)
        {
            _optixCylinders[materialId] = _context->createGeometry();
            _optixCylinders[materialId]->setPrimitiveCount(
                _timestampCylindersIndices[materialId]);
            _optixCylinders[materialId]->setBoundingBoxProgram(
                _cylindersBoundsProgram);
            _optixCylinders[materialId]->setIntersectionProgram(
                _cylindersIntersectProgram);
            uint64_t size = _timestampCylindersIndices[materialId] *
                            Cylinder::getSerializationSize();
            if (!_cylindersBuffers[materialId])
                _cylindersBuffers[materialId] =
                    _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT,
                                           size);
            memcpy(_cylindersBuffers[materialId]->map(),
                   &_serializedCylindersData[materialId][0],
                   size * sizeof(float));
            _cylindersBuffers[materialId]->unmap();
            _optixCylinders[materialId]["cylinders"]->setBuffer(
                _cylindersBuffers[materialId]);
            _geometryInstances.push_back(_context->createGeometryInstance(
                _optixCylinders[materialId], &_optixMaterials[materialId],
                &_optixMaterials[materialId] + 1));
            _serializedCylindersData[materialId].clear();
        }
    }

    const uint64_t memSize = _getBvhSize(
        totalNbCylinders * Cylinder::getSerializationSize() * sizeof(float));
    BRAYNS_DEBUG << "- Cylinders : " << totalNbCylinders << " [" << memSize
                 << " bytes]" << std::endl;
    return memSize;
}

uint64_t OptiXScene::_serializeCones()
{
    // Load geometry to GPU
    size_t totalNbCones = 0;
    for (size_t materialId = 0; materialId < _materials.size(); ++materialId)
    {
        _timestampConesIndices[materialId] = 0;

        if (_cones.find(materialId) != _cones.end())
        {
            for (const auto& cone : _cones[materialId])
            {
                cone->serializeData(_serializedConesData[materialId]);
                ++_serializedConesDataSize[materialId];
                ++_timestampConesIndices[materialId];
            }
            totalNbCones += _timestampConesIndices[materialId];
        }

        // Create cones geometry
        if (_timestampConesIndices[materialId] != 0)
        {
            _optixCones[materialId] = _context->createGeometry();
            _optixCones[materialId]->setPrimitiveCount(
                _timestampConesIndices[materialId]);
            _optixCones[materialId]->setBoundingBoxProgram(_conesBoundsProgram);
            _optixCones[materialId]->setIntersectionProgram(
                _conesIntersectProgram);
            uint64_t size = _timestampConesIndices[materialId] *
                            Cone::getSerializationSize();
            if (!_conesBuffers[materialId])
                _conesBuffers[materialId] =
                    _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT,
                                           size);
            memcpy(_conesBuffers[materialId]->map(),
                   &_serializedConesData[materialId][0], size * sizeof(float));
            _conesBuffers[materialId]->unmap();
            _optixCones[materialId]["cones"]->setBuffer(
                _conesBuffers[materialId]);
            _geometryInstances.push_back(_context->createGeometryInstance(
                _optixCones[materialId], &_optixMaterials[materialId],
                &_optixMaterials[materialId] + 1));
            _serializedConesData[materialId].clear();
        }
    }

    const uint64_t memSize = _getBvhSize(
        totalNbCones * Cone::getSerializationSize() * sizeof(float));
    BRAYNS_DEBUG << "- Cones     : " << totalNbCones << " [" << memSize
                 << " bytes]" << std::endl;
    return memSize;
}

uint64_t OptiXScene::serializeGeometry()
{
    const uint64_t spheresMemSize = _spheresDirty ? _serializeSpheres() : 0;
    const uint64_t cylindersMemSize =
        _cylindersDirty ? _serializeCylinders() : 0;
    const uint64_t conesMemSize = _conesDirty ? _serializeCones() : 0;

    _spheresDirty = false;
    _cylindersDirty = false;
    _conesDirty = false;
    return spheresMemSize + cylindersMemSize + conesMemSize;
}

uint64_t OptiXScene::_processMeshes()
{
    if (getVolumeHandler())
    {
        // Optix needs a bounding box around the volume so that if can find
        // intersections
        // before initiating the traversal
        _processVolumeAABBGeometry();
    }

    // These buffers will be shared across all Geometries
    uint64_t size = 0;

    uint64_t nbTotalVertices = 0;
    uint64_t nbTotalIndices = 0;
    uint64_t nbTotalNormals = 0;
    uint64_t nbTotalTexCoords = 0;
    uint64_t nbTotalMaterials = 0;

    for (size_t materialId = 0; materialId < _materials.size(); ++materialId)
        if (_trianglesMeshes.find(materialId) != _trianglesMeshes.end())
        {
            nbTotalVertices +=
                _trianglesMeshes[materialId].getVertices().size();
            nbTotalIndices += _trianglesMeshes[materialId].getIndices().size();
            nbTotalNormals += _trianglesMeshes[materialId].getNormals().size();
            nbTotalTexCoords +=
                _trianglesMeshes[materialId].getTextureCoordinates().size();
        }

    Vector3fs vertices;
    Vector3uis indices;
    Vector3fs normals;
    Vector2fs texCoords;
    ints materials;

    if (nbTotalIndices == 0)
    {
        BRAYNS_INFO << "- No meshes" << std::endl;
        _verticesBuffer =
            _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, 0);
        _context["vertices_buffer"]->setBuffer(_verticesBuffer);
        _indicesBuffer =
            _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_INT3, 0);
        _context["indices_buffer"]->setBuffer(_indicesBuffer);
        _normalsBuffer =
            _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, 0);
        _context["normal_buffer"]->setBuffer(_normalsBuffer);
        _textureCoordsBuffer =
            _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT2, 0);
        _context["texcoord_buffer"]->setBuffer(_textureCoordsBuffer);
        _materialsBuffer =
            _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_INT, 0);
        _context["material_buffer"]->setBuffer(_materialsBuffer);
        return 0;
    }

    uint64_t offset = 0;
    for (size_t materialId = 0; materialId < _materials.size(); ++materialId)
    {
        if (_trianglesMeshes[materialId].getIndices().size() != 0)
        {
            // Vertices
            const Vector3fs& v = _trianglesMeshes[materialId].getVertices();
            vertices.insert(vertices.end(), v.begin(), v.end());

            // Indices
            for (size_t i = 0;
                 i < _trianglesMeshes[materialId].getIndices().size(); ++i)
            {
                Vector3ui value = _trianglesMeshes[materialId].getIndices()[i];
                value += offset;
                indices.push_back(value);
                materials.push_back(materialId);
            }
            _trianglesMeshes[materialId].getIndices().clear();

            // Normals
            const Vector3fs& n = _trianglesMeshes[materialId].getNormals();
            normals.insert(normals.end(), n.begin(), n.end());
            _trianglesMeshes[materialId].getNormals().clear();

            // Texture coordinates
            const Vector2fs& t =
                _trianglesMeshes[materialId].getTextureCoordinates();
            texCoords.insert(texCoords.end(), t.begin(), t.end());
            _trianglesMeshes[materialId].getTextureCoordinates().clear();

            offset += _trianglesMeshes[materialId].getVertices().size();
            _trianglesMeshes[materialId].getVertices().clear();
        }
    }

    _verticesBuffer = _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3,
                                             vertices.size());
    size = vertices.size() * 3 * sizeof(float);
    if (size != 0)
    {
        memcpy(_verticesBuffer->map(), &vertices.data()[0], size);
        _verticesBuffer->unmap();
    }

    _indicesBuffer =
        _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_INT3, indices.size());
    size = indices.size() * 3 * sizeof(int);
    if (size != 0)
    {
        memcpy(_indicesBuffer->map(), &indices.data()[0], size);
        _indicesBuffer->unmap();
    }

    _normalsBuffer = _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3,
                                            normals.size());
    size = normals.size() * 3 * sizeof(float);
    if (size != 0)
    {
        memcpy(_normalsBuffer->map(), &normals.data()[0], size);
        _normalsBuffer->unmap();
    }

    _textureCoordsBuffer =
        _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT2,
                               texCoords.size());
    size = texCoords.size() * 2 * sizeof(float);
    if (size != 0)
    {
        memcpy(_textureCoordsBuffer->map(), &texCoords.data()[0], size);
        _textureCoordsBuffer->unmap();
    }

    size = materials.size() * sizeof(int);
    _materialsBuffer = _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_INT,
                                              materials.size());
    if (size != 0)
    {
        memcpy(_materialsBuffer->map(), &materials.data()[0], size);
        _materialsBuffer->unmap();
    }
    nbTotalMaterials = materials.size();

    // Attach all of the buffers at the Context level since they are shared
    _context["vertices_buffer"]->setBuffer(_verticesBuffer);
    _context["indices_buffer"]->setBuffer(_indicesBuffer);
    _context["normal_buffer"]->setBuffer(_normalsBuffer);
    _context["texcoord_buffer"]->setBuffer(_textureCoordsBuffer);
    _context["material_buffer"]->setBuffer(_materialsBuffer);

    _mesh = _context->createGeometry();
    _mesh->setIntersectionProgram(_meshIntersectProgram);
    _mesh->setBoundingBoxProgram(_meshBoundsProgram);
    _mesh->setPrimitiveCount(nbTotalIndices);

    _geometryInstances.push_back(
        _context->createGeometryInstance(_mesh, &_optixMaterials[0],
                                         &_optixMaterials[0] +
                                             _optixMaterials.size()));

    const uint64_t verticesMemSize = nbTotalVertices * 3 * sizeof(float);
    const uint64_t indicesMemSize = nbTotalIndices * 3 * sizeof(int);
    const uint64_t normalsMemSize = nbTotalNormals * 3 * sizeof(float);
    const uint64_t texCoordsMemSize = nbTotalTexCoords * 2 * sizeof(float);
    const uint64_t materialsMemSize = nbTotalMaterials * sizeof(int);

    BRAYNS_INFO << "- Vertices  : " << vertices.size() << " ["
                << verticesMemSize << " bytes]" << std::endl;
    BRAYNS_INFO << "- Indices   : " << indices.size() << " [" << indicesMemSize
                << " bytes]" << std::endl;
    BRAYNS_INFO << "- Normals   : " << normals.size() << " [" << normalsMemSize
                << " bytes]" << std::endl;
    BRAYNS_INFO << "- Tex coords: " << texCoords.size() << " ["
                << texCoordsMemSize << " bytes]" << std::endl;
    BRAYNS_INFO << "- Materials : " << materials.size() << " ["
                << materialsMemSize << " bytes]" << std::endl;

    const uint64_t bvhSize = _getBvhSize(indicesMemSize / 3);

    return bvhSize + verticesMemSize + indicesMemSize + normalsMemSize +
           texCoordsMemSize + materialsMemSize;
}

void OptiXScene::commitMaterials(const bool updateOnly)
{
    BRAYNS_INFO << "Commit OptiX materials" << std::endl;

    // Create materials
    for (size_t materialId = 0; materialId < _materials.size(); ++materialId)
        if (materialId >= _optixMaterials.size())
        {
            optix::Material optixMaterial = _context->createMaterial();
            optixMaterial->setAnyHitProgram(1, _phong_ah);
            _optixMaterials.push_back(optixMaterial);
        }

    // Load textures once and for all (textures are ignored by the material
    // update process
    if (!updateOnly)
    {
        // Load textures from disk
        for (size_t materialId = 0; materialId < _materials.size();
             ++materialId)
        {
            bool textured = false;
            optix::Material optixMaterial = _optixMaterials[materialId];
            MaterialPtr material = _materials[materialId];
            for (const auto texture : material->getTextures())
            {
                BRAYNS_ERROR << "Texture nane: " << texture.second << std::endl;
                TextureLoader textureLoader;
                if (texture.second != TEXTURE_NAME_SIMULATION)
                    if (textureLoader.loadTexture(_textures, texture.first,
                                                  texture.second))
                    {
                        optixMaterial->setClosestHitProgram(0,
                                                            _phong_ch_textured);
                        textured = true;
                    }

                if (_createTexture2D(texture.second))
                {
                    if (materialId == MATERIAL_SKYBOX)
                        _context["envmap"]->setTextureSampler(
                            _optixTextureSamplers[texture.second]);

                    optixMaterial["diffuse_map"]->setTextureSampler(
                        _optixTextureSamplers[texture.second]);

                    BRAYNS_INFO << "Texture " << texture.second
                                << " assigned to material " << materialId
                                << std::endl;
                }
            }
            if (!textured)
                optixMaterial->setClosestHitProgram(0, _phong_ch);
        }
    }

    // Populate material attributes
    for (size_t materialId = 0; materialId < _materials.size(); ++materialId)
    {
        // Material
        MaterialPtr material = _materials[materialId];
        assert(material);

        optix::Material optixMaterial = _optixMaterials[materialId];

        const Vector3f& color = material->getColor();
        optixMaterial["Kd"]->setFloat(color.z(), color.y(), color.x());
        const Vector3f& specularColor = material->getSpecularColor();
        optixMaterial["Ks"]->setFloat(specularColor.z(), specularColor.y(),
                                      specularColor.x());
        optixMaterial["phong_exp"]->setFloat(material->getSpecularExponent());
        const float reflectionIndex = material->getReflectionIndex();
        optixMaterial["Kr"]->setFloat(reflectionIndex, reflectionIndex,
                                      reflectionIndex);
        const float opacity = material->getOpacity();
        optixMaterial["Ko"]->setFloat(opacity, opacity, opacity);
        optixMaterial["refraction_index"]->setFloat(
            material->getRefractionIndex());
    }
}

bool OptiXScene::_createTexture2D(const std::string& textureName)
{
    if (_optixTextures.find(textureName) != _optixTextures.end())
        return false;
    Texture2DPtr tex = _textures[textureName];

    const uint16_t nx = tex->getWidth();
    const uint16_t ny = tex->getHeight();
    const uint16_t channels = tex->getNbChannels();
    const uint16_t optixChannels = 4;

    // Create texture sampler
    optix::TextureSampler sampler = _context->createTextureSampler();
    sampler->setWrapMode(0, RT_WRAP_REPEAT);
    sampler->setWrapMode(1, RT_WRAP_REPEAT);
    sampler->setWrapMode(2, RT_WRAP_REPEAT);
    sampler->setIndexingMode(RT_TEXTURE_INDEX_NORMALIZED_COORDINATES);
    sampler->setReadMode(RT_TEXTURE_READ_NORMALIZED_FLOAT);
    sampler->setMaxAnisotropy(1.0f);
    sampler->setMipLevelCount(1u);
    sampler->setArraySize(1u);

    // Create buffer and populate with texture data
    optix::Buffer buffer =
        _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT4, nx, ny);
    float* buffer_data = static_cast<float*>(buffer->map());

    size_t idx_src = 0;
    size_t idx_dst = 0;
    for (uint16_t y = 0; y < ny; ++y)
    {
        for (uint16_t x = 0; x < nx; ++x)
        {
            buffer_data[idx_dst + 0] =
                float(tex->getRawData()[idx_src + 2]) / 255.f;
            buffer_data[idx_dst + 1] =
                float(tex->getRawData()[idx_src + 1]) / 255.f;
            buffer_data[idx_dst + 2] =
                float(tex->getRawData()[idx_src + 0]) / 255.f;
            buffer_data[idx_dst + 3] =
                (channels == optixChannels)
                    ? float(tex->getRawData()[idx_src + 4]) / 255.f
                    : 1.f;
            idx_dst += 4;
            idx_src += (channels == optixChannels) ? 4 : 3;
        }
    }
    buffer->unmap();
    _optixTextures[textureName] = buffer;

    // Assign buffer to sampler
    sampler->setBuffer(buffer);
    sampler->setFilteringModes(RT_FILTER_LINEAR, RT_FILTER_LINEAR,
                               RT_FILTER_NONE);
    _optixTextureSamplers[textureName] = sampler;
    return true;
}

void OptiXScene::commitSimulationData()
{
    BRAYNS_ERROR << "OptiXScene::commitSimulationData not implemented"
                 << std::endl;
}

void OptiXScene::commitVolumeData()
{
    const float timestamp =
        _parametersManager.getSceneParameters().getTimestamp();
    VolumeHandlerPtr volumeHandler = getVolumeHandler();
    if (!volumeHandler || !volumeHandler->getData())
    {
        _volumeBuffer =
            _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_UNSIGNED_BYTE, 0);
        _context["volumeData"]->setBuffer(_volumeBuffer);
        _context["volumeDimensions"]->setUint(0, 0, 0);
        _context["volumeScale"]->setFloat(0.f, 0.f, 0.f);
        _context["volumePosition"]->setFloat(0.f, 0.f, 0.f);
        _context["volumeEpsilon"]->setFloat(0.f);
        _context["volumeDiag"]->setFloat(0.f);
        return;
    }

    volumeHandler->setTimestamp(timestamp);
    void* data = volumeHandler->getData();
    if (data)
    {
        if (_volumeBuffer)
            _volumeBuffer->destroy();

        _volumeBuffer =
            _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_UNSIGNED_BYTE,
                                   volumeHandler->getSize());
        uint64_t size = volumeHandler->getSize() * sizeof(unsigned char);
        if (size != 0)
        {
            memcpy(_volumeBuffer->map(), data, size);
            _volumeBuffer->unmap();
        }
        _context["volumeData"]->setBuffer(_volumeBuffer);
    }

    const Vector3ui& dimensions = volumeHandler->getDimensions();
    _context["volumeDimensions"]->setUint(dimensions.x(), dimensions.y(),
                                          dimensions.z());

    const Vector3f& elementSpacing = volumeHandler->getElementSpacing();
    _context["volumeElementSpacing"]->setFloat(elementSpacing.x(),
                                               elementSpacing.y(),
                                               elementSpacing.z());

    const Vector3f& position = volumeHandler->getOffset();
    _context["volumeOffset"]->setFloat(position.x(), position.y(),
                                       position.z());

    _context["volumeEpsilon"]->setFloat(volumeHandler->getEpsilon(
        elementSpacing,
        _parametersManager.getVolumeParameters().getSamplesPerRay()));

    const Vector3f diag = Vector3f(dimensions) * elementSpacing;
    const float volumeDiag = diag.find_max();
    _context["volumeDiag"]->setFloat(volumeDiag);
}

void OptiXScene::commitTransferFunctionData()
{
    if (!_colorMapBuffer)
        _colorMapBuffer =
            _context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT4,
                                   _transferFunction.getDiffuseColors().size());

    uint64_t size =
        _transferFunction.getDiffuseColors().size() * 4 * sizeof(float);
    if (size != 0)
    {
        memcpy(_colorMapBuffer->map(), &_transferFunction.getDiffuseColors()[0],
               size);
        _colorMapBuffer->unmap();
    }

    _context["colorMap"]->setBuffer(_colorMapBuffer);
    _context["colorMapMinValue"]->setFloat(
        _transferFunction.getValuesRange().x());
    _context["colorMapRange"]->setFloat(_transferFunction.getValuesRange().y() -
                                        _transferFunction.getValuesRange().x());
    _context["colorMapSize"]->setUint(
        _transferFunction.getDiffuseColors().size());
}

void OptiXScene::_processVolumeAABBGeometry()
{
    VolumeHandlerPtr volumeHandler = getVolumeHandler();
    if (!volumeHandler)
        return;

    const Vector3f positions[8] = {
        {0.f, 0.f, 0.f}, {1.f, 0.f, 0.f}, //    6--------7
        {0.f, 1.f, 0.f},                  //   /|       /|
        {1.f, 1.f, 0.f},                  //  2--------3 |
        {0.f, 0.f, 1.f},                  //  | |      | |
        {1.f, 0.f, 1.f},                  //  | 4------|-5
        {0.f, 1.f, 1.f},                  //  |/       |/
        {1.f, 1.f, 1.f}                   //  0--------1
    };

    const uint16_t indices[6][6] = {
        {0, 1, 3, 3, 2, 0}, // Front
        {1, 5, 7, 7, 3, 1}, // Right
        {5, 4, 6, 6, 7, 5}, // Back
        {4, 0, 2, 2, 6, 4}, // Left
        {0, 1, 5, 5, 4, 0}, // Bottom
        {2, 3, 7, 7, 6, 2}  // Top
    };

    const float timestamp =
        _parametersManager.getSceneParameters().getTimestamp();
    volumeHandler->setTimestamp(timestamp);
    const Vector3f& volumeElementSpacing = volumeHandler->getElementSpacing();
    const Vector3f& volumeOffset = volumeHandler->getOffset();
    const Vector3ui& volumeDimensions = volumeHandler->getDimensions();

    uint64_t offset = _trianglesMeshes[MATERIAL_INVISIBLE].getVertices().size();
    for (size_t face = 0; face < 6; ++face)
    {
        for (size_t index = 0; index < 6; ++index)
        {
            const Vector3f position = positions[indices[face][index]] *
                                          volumeElementSpacing *
                                          volumeDimensions +
                                      volumeOffset;

            _trianglesMeshes[MATERIAL_INVISIBLE].getVertices().push_back(
                position);

            _bounds.merge(position);
        }
        const size_t index = offset + face * 6;
        _trianglesMeshes[MATERIAL_INVISIBLE].getIndices().push_back(
            Vector3ui(index + 0, index + 1, index + 2));
        _trianglesMeshes[MATERIAL_INVISIBLE].getIndices().push_back(
            Vector3ui(index + 3, index + 4, index + 5));
    }
}

uint64_t OptiXScene::_getBvhSize(const uint64_t nbElements) const
{
    // An estimate of the BVH size
    return 64 * 1.3f * nbElements;
}

void OptiXScene::saveSceneToCacheFile()
{
    BRAYNS_ERROR << "OptiXScene::saveSceneToCacheFile not implemented"
                 << std::endl;
}

bool OptiXScene::isVolumeSupported(const std::string& volumeFile) const
{
    return boost::algorithm::ends_with(volumeFile, ".raw");
}
}
