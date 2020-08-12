/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
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

#include "OptiXModel.h"
#include "OptiXContext.h"
#include "OptiXMaterial.h"

#include <brayns/common/log.h>
#include <brayns/common/simulation/AbstractSimulationHandler.h>
#include <brayns/parameters/AnimationParameters.h>

#include <brayns/engineapi/Material.h>

namespace brayns
{
template <typename T>
void setBufferRaw(RTbuffertype bufferType, RTformat bufferFormat,
                  optix::Handle<optix::BufferObj>& buffer,
                  optix::Handle<optix::VariableObj> geometry, T* src,
                  const size_t numElements, const size_t bytes)
{
    auto context = OptiXContext::get().getOptixContext();
    if (!buffer)
        buffer = context->createBuffer(bufferType, bufferFormat, numElements);
    else
        buffer->setSize(numElements);
    if (src != nullptr && numElements > 0 && bytes > 0)
    {
        memcpy(buffer->map(0, RT_BUFFER_MAP_WRITE_DISCARD), src, bytes);
        buffer->unmap();
    }
    geometry->setBuffer(buffer);
}

template <typename T>
void setBuffer(RTbuffertype bufferType, RTformat bufferFormat,
               optix::Handle<optix::BufferObj>& buffer,
               optix::Handle<optix::VariableObj> geometry,
               const std::vector<T>& src, const size_t numElements)
{
    setBufferRaw(bufferType, bufferFormat, buffer, geometry, src.data(),
                 numElements, sizeof(T) * src.size());
}

OptiXModel::OptiXModel(AnimationParameters& animationParameters,
                       VolumeParameters& volumeParameters)
    : Model(animationParameters, volumeParameters)
{
}

void OptiXModel::commitGeometry()
{
    // Materials
    _commitMaterials();

    const auto compactBVH = getBVHFlags().count(BVHFlag::compact) > 0;
    // Geometry group
    if (!_geometryGroup)
        _geometryGroup = OptiXContext::get().createGeometryGroup(compactBVH);

    // Bounding box group
    if (!_boundingBoxGroup)
        _boundingBoxGroup = OptiXContext::get().createGeometryGroup(compactBVH);

    size_t nbSpheres = 0;
    size_t nbCylinders = 0;
    size_t nbCones = 0;
    if (_spheresDirty)
    {
        for (const auto& spheres : _geometries->_spheres)
        {
            nbSpheres += spheres.second.size();
            _commitSpheres(spheres.first);
        }
        BRAYNS_DEBUG << nbSpheres << " spheres" << std::endl;
    }

    if (_cylindersDirty)
    {
        for (const auto& cylinders : _geometries->_cylinders)
        {
            nbCylinders += cylinders.second.size();
            _commitCylinders(cylinders.first);
        }
        BRAYNS_DEBUG << nbCylinders << " cylinders" << std::endl;
    }

    if (_conesDirty)
    {
        for (const auto& cones : _geometries->_cones)
        {
            nbCones += cones.second.size();
            _commitCones(cones.first);
        }
        BRAYNS_DEBUG << nbCones << " cones" << std::endl;
    }

    if (_triangleMeshesDirty)
        for (const auto& meshes : _geometries->_triangleMeshes)
            _commitMeshes(meshes.first);

    updateBounds();
    _markGeometriesClean();

    // handled by the scene
    _instancesDirty = false;

    BRAYNS_DEBUG << "Geometry group has " << _geometryGroup->getChildCount()
                 << " children instances" << std::endl;
    BRAYNS_DEBUG << "Bounding box group has "
                 << _boundingBoxGroup->getChildCount() << " children instances"
                 << std::endl;
}

void OptiXModel::_commitSpheres(const size_t materialId)
{
    if (_geometries->_spheres.find(materialId) == _geometries->_spheres.end())
        return;

    auto context = OptiXContext::get().getOptixContext();
    const auto& spheres = _geometries->_spheres[materialId];
    context["sphere_size"]->setUint(sizeof(Sphere) / sizeof(float));

    // Geometry
    _optixSpheres[materialId] =
        OptiXContext::get().createGeometry(OptixGeometryType::sphere);
    _optixSpheres[materialId]->setPrimitiveCount(spheres.size());

    setBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT, _spheresBuffers[materialId],
              _optixSpheres[materialId]["spheres"], spheres,
              sizeof(Sphere) * spheres.size());

    // Material
    auto& mat = static_cast<OptiXMaterial&>(*_materials[materialId]);
    const auto material = mat.getOptixMaterial();
    if (!material)
        BRAYNS_THROW(std::runtime_error("Material is not defined"));

    // Instance
    auto instance = context->createGeometryInstance();
    instance->setGeometry(_optixSpheres[materialId]);
    instance->setMaterialCount(1);
    instance->setMaterial(0, material);
    if (materialId == BOUNDINGBOX_MATERIAL_ID)
        _boundingBoxGroup->addChild(instance);
    else
        _geometryGroup->addChild(instance);
}

void OptiXModel::_commitCylinders(const size_t materialId)
{
    if (_geometries->_cylinders.find(materialId) ==
        _geometries->_cylinders.end())
        return;

    auto context = OptiXContext::get().getOptixContext();
    const auto& cylinders = _geometries->_cylinders[materialId];
    context["cylinder_size"]->setUint(sizeof(Cylinder) / sizeof(float));
    _optixCylinders[materialId] =
        OptiXContext::get().createGeometry(OptixGeometryType::cylinder);

    auto& optixCylinders = _optixCylinders[materialId];
    optixCylinders->setPrimitiveCount(cylinders.size());

    setBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT, _cylindersBuffers[materialId],
              _optixCylinders[materialId]["cylinders"], cylinders,
              sizeof(Cylinder) * cylinders.size());

    auto& mat = static_cast<OptiXMaterial&>(*_materials[materialId]);
    const auto material = mat.getOptixMaterial();
    if (!material)
        BRAYNS_THROW(std::runtime_error("Material is not defined"));

    auto instance = context->createGeometryInstance();
    instance->setGeometry(optixCylinders);
    instance->setMaterialCount(1);
    instance->setMaterial(0, material);
    if (materialId == BOUNDINGBOX_MATERIAL_ID)
        _boundingBoxGroup->addChild(instance);
    else
        _geometryGroup->addChild(instance);
}

void OptiXModel::_commitCones(const size_t materialId)
{
    if (_geometries->_cones.find(materialId) == _geometries->_cones.end())
        return;

    auto context = OptiXContext::get().getOptixContext();
    const auto& cones = _geometries->_cones[materialId];
    context["cone_size"]->setUint(sizeof(Cone) / sizeof(float));
    _optixCones[materialId] =
        OptiXContext::get().createGeometry(OptixGeometryType::cone);

    auto& optixCones = _optixCones[materialId];
    optixCones->setPrimitiveCount(cones.size());

    setBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT, _conesBuffers[materialId],
              _optixCones[materialId]["cones"], cones,
              sizeof(Cone) * cones.size());

    auto& mat = static_cast<OptiXMaterial&>(*_materials[materialId]);
    auto material = mat.getOptixMaterial();
    if (!material)
        BRAYNS_THROW(std::runtime_error("Material is not defined"));

    auto instance = context->createGeometryInstance();
    instance->setGeometry(optixCones);
    instance->setMaterialCount(1);
    instance->setMaterial(0, material);
    if (materialId == BOUNDINGBOX_MATERIAL_ID)
        _boundingBoxGroup->addChild(instance);
    else
        _geometryGroup->addChild(instance);
}

void OptiXModel::_commitMeshes(const size_t materialId)
{
    if (_geometries->_triangleMeshes.find(materialId) ==
        _geometries->_triangleMeshes.end())
        return;

    const auto& meshes = _geometries->_triangleMeshes[materialId];
    _optixMeshes[materialId] =
        OptiXContext::get().createGeometry(OptixGeometryType::triangleMesh);

    auto& optixMeshes = _optixMeshes[materialId];
    optixMeshes->setPrimitiveCount(meshes.indices.size());

    setBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3,
              _meshesBuffers[materialId].vertices_buffer,
              _optixMeshes[materialId]["vertices_buffer"], meshes.vertices,
              meshes.vertices.size());

    setBuffer(RT_BUFFER_INPUT, RT_FORMAT_UNSIGNED_INT3,
              _meshesBuffers[materialId].indices_buffer,
              _optixMeshes[materialId]["indices_buffer"], meshes.indices,
              meshes.indices.size());

    setBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3,
              _meshesBuffers[materialId].normal_buffer,
              _optixMeshes[materialId]["normal_buffer"], meshes.normals,
              meshes.normals.size());

    setBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT2,
              _meshesBuffers[materialId].texcoord_buffer,
              _optixMeshes[materialId]["texcoord_buffer"],
              meshes.textureCoordinates, meshes.textureCoordinates.size());

    auto& mat = static_cast<OptiXMaterial&>(*_materials[materialId]);
    auto material = mat.getOptixMaterial();
    if (!material)
        BRAYNS_THROW(std::runtime_error("Material is not defined"));

    auto context = OptiXContext::get().getOptixContext();
    auto instance = context->createGeometryInstance();
    instance->setGeometry(optixMeshes);
    instance->setMaterialCount(1);
    instance->setMaterial(0, material);
    if (materialId == BOUNDINGBOX_MATERIAL_ID)
        _boundingBoxGroup->addChild(instance);
    else
        _geometryGroup->addChild(instance);
}

void OptiXModel::_commitMaterials()
{
    BRAYNS_INFO << "Committing " << _materials.size() << " OptiX materials"
                << std::endl;

    for (auto& material : _materials)
        material.second->commit();
}

void OptiXModel::buildBoundingBox()
{
    if (_boundingBoxBuilt)
        return;

    _boundingBoxBuilt = true;

    auto material = createMaterial(BOUNDINGBOX_MATERIAL_ID, "bounding_box");
    material->setDiffuseColor({1, 1, 1});
    material->setEmission(1.f);

    const Vector3f s(0.5f);
    const Vector3f c(0.5f);
    const float radius = 0.005f;
    const Vector3f positions[8] = {
        {c.x - s.x, c.y - s.y, c.z - s.z},
        {c.x + s.x, c.y - s.y, c.z - s.z}, //    6--------7
        {c.x - s.x, c.y + s.y, c.z - s.z}, //   /|       /|
        {c.x + s.x, c.y + s.y, c.z - s.z}, //  2--------3 |
        {c.x - s.x, c.y - s.y, c.z + s.z}, //  | |      | |
        {c.x + s.x, c.y - s.y, c.z + s.z}, //  | 4------|-5
        {c.x - s.x, c.y + s.y, c.z + s.z}, //  |/       |/
        {c.x + s.x, c.y + s.y, c.z + s.z}  //  0--------1
    };

    for (size_t i = 0; i < 8; ++i)
        addSphere(BOUNDINGBOX_MATERIAL_ID, Sphere(positions[i], radius));

    addCylinder(BOUNDINGBOX_MATERIAL_ID, {positions[0], positions[1], radius});
    addCylinder(BOUNDINGBOX_MATERIAL_ID, {positions[2], positions[3], radius});
    addCylinder(BOUNDINGBOX_MATERIAL_ID, {positions[4], positions[5], radius});
    addCylinder(BOUNDINGBOX_MATERIAL_ID, {positions[6], positions[7], radius});

    addCylinder(BOUNDINGBOX_MATERIAL_ID, {positions[0], positions[2], radius});
    addCylinder(BOUNDINGBOX_MATERIAL_ID, {positions[1], positions[3], radius});
    addCylinder(BOUNDINGBOX_MATERIAL_ID, {positions[4], positions[6], radius});
    addCylinder(BOUNDINGBOX_MATERIAL_ID, {positions[5], positions[7], radius});

    addCylinder(BOUNDINGBOX_MATERIAL_ID, {positions[0], positions[4], radius});
    addCylinder(BOUNDINGBOX_MATERIAL_ID, {positions[1], positions[5], radius});
    addCylinder(BOUNDINGBOX_MATERIAL_ID, {positions[2], positions[6], radius});
    addCylinder(BOUNDINGBOX_MATERIAL_ID, {positions[3], positions[7], radius});
}

MaterialPtr OptiXModel::createMaterialImpl(
    const PropertyMap& properties BRAYNS_UNUSED)
{
    auto material = std::make_shared<OptiXMaterial>();
    if (!material)
        BRAYNS_THROW(std::runtime_error("Failed to create material"));
    return material;
}

/** @copydoc Model::createSharedDataVolume */
SharedDataVolumePtr OptiXModel::createSharedDataVolume(
    const Vector3ui& /*dimensions*/, const Vector3f& /*spacing*/,
    const DataType /*type*/) const
{
    throw std::runtime_error("Not implemented");
    return nullptr;
}

/** @copydoc Model::createBrickedVolume */
BrickedVolumePtr OptiXModel::createBrickedVolume(
    const Vector3ui& /*dimensions*/, const Vector3f& /*spacing*/,
    const DataType /*type*/) const
{
    throw std::runtime_error("Not implemented");
    return nullptr;
}

void OptiXModel::_commitTransferFunctionImpl(const Vector3fs& colors,
                                             const floats& opacities,
                                             const Vector2d valueRange)
{
    auto context = OptiXContext::get().getOptixContext();

    setBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, _optixTransferFunction.colors,
              context["colors"], colors, colors.size());

    setBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT,
              _optixTransferFunction.opacities, context["opacities"], opacities,
              opacities.size());

    context["value_range"]->setFloat(valueRange.x, valueRange.y);
}

void OptiXModel::_commitSimulationDataImpl(const float* frameData,
                                           const size_t frameSize)
{
    auto context = OptiXContext::get().getOptixContext();
    setBufferRaw(RT_BUFFER_INPUT, RT_FORMAT_FLOAT, _simulationData,
                 context["simulation_data"], frameData, frameSize,
                 frameSize * sizeof(float));
}
} // namespace brayns
