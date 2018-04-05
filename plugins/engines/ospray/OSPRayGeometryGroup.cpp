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

#include "OSPRayGeometryGroup.h"

namespace brayns
{
OSPRayGeometryGroup::OSPRayGeometryGroup(MaterialManagerPtr materialManager)
    : GeometryGroup(materialManager)
{
    BRAYNS_FCT_ENTRY
}

void OSPRayGeometryGroup::setMemoryFlags(const size_t memoryManagementFlags)
{
    BRAYNS_FCT_ENTRY
    _memoryManagementFlags = memoryManagementFlags;
}

OSPRayGeometryGroup::~OSPRayGeometryGroup()
{
    BRAYNS_FCT_ENTRY
    unload();
}

void OSPRayGeometryGroup::unload()
{
    BRAYNS_FCT_ENTRY

    for (auto& geom : _ospExtendedSpheres)
        ospRelease(geom.second);
    _ospExtendedSpheres.clear();
    for (auto& geom : _ospExtendedSpheresData)
        ospRelease(geom.second);
    _ospExtendedSpheresData.clear();
    for (auto& geom : _ospExtendedCylinders)
        ospRelease(geom.second);
    _ospExtendedCylinders.clear();
    for (auto& geom : _ospExtendedCylindersData)
        ospRelease(geom.second);
    _ospExtendedCylindersData.clear();
    for (auto& geom : _ospExtendedCones)
        ospRelease(geom.second);
    _ospExtendedCones.clear();
    for (auto& geom : _ospExtendedConesData)
        ospRelease(geom.second);
    _ospExtendedConesData.clear();
    for (auto& geom : _ospMeshes)
        ospRelease(geom.second);
    _ospMeshes.clear();

    if (_useSimulationModel)
    {
        for (auto geom : _ospExtendedSpheres)
            ospRemoveGeometry(_simulationModel, geom.second);
        for (auto geom : _ospExtendedCylinders)
            ospRemoveGeometry(_simulationModel, geom.second);
        for (auto geom : _ospExtendedCones)
            ospRemoveGeometry(_simulationModel, geom.second);
    }
    else
    {
        for (auto geom : _ospExtendedSpheres)
            ospRemoveGeometry(_model, geom.second);
        for (auto geom : _ospExtendedCylinders)
            ospRemoveGeometry(_model, geom.second);
        for (auto geom : _ospExtendedCones)
            ospRemoveGeometry(_model, geom.second);
    }

    if (_model)
    {
        ospCommit(_model);
        ospRelease(_model);
    }
    _model = nullptr;

    if (_simulationModel)
    {
        ospCommit(_simulationModel);
        ospRelease(_simulationModel);
    }
    _simulationModel = nullptr;

    _updateValue(_spheresDirty, true);
    _updateValue(_cylindersDirty, true);
    _updateValue(_conesDirty, true);
    _updateValue(_trianglesMeshesDirty, true);
}

uint64_t OSPRayGeometryGroup::_commitSpheres(const size_t materialId)
{
    BRAYNS_FCT_ENTRY
    const auto& spheres = _spheres[materialId];
    const auto bufferSize = spheres.size() * sizeof(Sphere);
    if (_ospExtendedSpheres.find(materialId) != _ospExtendedSpheres.end())
        ospRemoveGeometry(_model, _ospExtendedSpheres[materialId]);

    _ospExtendedSpheres[materialId] = ospNewGeometry("extendedspheres");
    _ospExtendedSpheresData[materialId] =
        ospNewData(bufferSize / sizeof(float), OSP_FLOAT, spheres.data(),
                   _memoryManagementFlags);

    ospSetObject(_ospExtendedSpheres[materialId], "extendedspheres",
                 _ospExtendedSpheresData[materialId]);

    auto impl =
        std::static_pointer_cast<OSPRayMaterialManager>(_materialManager);
    const auto& ospMaterial = impl->getOSPMaterial(materialId);
    ospSetMaterial(_ospExtendedSpheres[materialId], ospMaterial);

    ospCommit(_ospExtendedSpheres[materialId]);

    if (_useSimulationModel)
        ospAddGeometry(_simulationModel, _ospExtendedSpheres[materialId]);
    else
        ospAddGeometry(_model, _ospExtendedSpheres[materialId]);

    _updateValue(_spheresDirty, false);
    return bufferSize;
}

uint64_t OSPRayGeometryGroup::_commitCylinders(const size_t materialId)
{
    BRAYNS_FCT_ENTRY
    const auto& cylinders = _cylinders[materialId];
    const auto bufferSize = cylinders.size() * sizeof(Cylinder);
    if (_ospExtendedCylinders.find(materialId) != _ospExtendedCylinders.end())
        ospRemoveGeometry(_model, _ospExtendedCylinders[materialId]);

    _ospExtendedCylinders[materialId] = ospNewGeometry("extendedcylinders");
    _ospExtendedCylindersData[materialId] =
        ospNewData(bufferSize / sizeof(float), OSP_FLOAT, cylinders.data(),
                   _memoryManagementFlags);
    ospSetObject(_ospExtendedCylinders[materialId], "extendedcylinders",
                 _ospExtendedCylindersData[materialId]);

    auto impl =
        std::static_pointer_cast<OSPRayMaterialManager>(_materialManager);
    const auto& ospMaterial = impl->getOSPMaterial(materialId);
    ospSetMaterial(_ospExtendedCylinders[materialId], ospMaterial);

    ospCommit(_ospExtendedCylinders[materialId]);

    if (_useSimulationModel)
        ospAddGeometry(_simulationModel, _ospExtendedCylinders[materialId]);
    else
        ospAddGeometry(_model, _ospExtendedCylinders[materialId]);
    _updateValue(_cylindersDirty, false);
    return bufferSize;
}

uint64_t OSPRayGeometryGroup::_commitCones(const size_t materialId)
{
    BRAYNS_FCT_ENTRY
    const auto& cones = _cones[materialId];
    const auto bufferSize = cones.size() * sizeof(Cone);
    if (_ospExtendedCones.find(materialId) != _ospExtendedCones.end())
        ospRemoveGeometry(_model, _ospExtendedCones[materialId]);

    _ospExtendedCones[materialId] = ospNewGeometry("extendedcones");
    _ospExtendedConesData[materialId] =
        ospNewData(bufferSize / sizeof(float), OSP_FLOAT, cones.data(),
                   _memoryManagementFlags);
    ospSetObject(_ospExtendedCones[materialId], "extendedcones",
                 _ospExtendedConesData[materialId]);

    auto impl =
        std::static_pointer_cast<OSPRayMaterialManager>(_materialManager);
    const auto& ospMaterial = impl->getOSPMaterial(materialId);
    ospSetMaterial(_ospExtendedCones[materialId], ospMaterial);

    ospCommit(_ospExtendedCones[materialId]);

    if (_useSimulationModel)
        ospAddGeometry(_simulationModel, _ospExtendedCones[materialId]);
    else
        ospAddGeometry(_model, _ospExtendedCones[materialId]);
    _updateValue(_conesDirty, false);
    return bufferSize;
}

uint64_t OSPRayGeometryGroup::_commitMeshes(const size_t materialId)
{
    BRAYNS_FCT_ENTRY
    uint64_t size = 0;
    _ospMeshes[materialId] = ospNewGeometry("trianglemesh");

    auto& trianglesMesh = _trianglesMeshes[materialId];
    size += trianglesMesh.vertices.size() * 3 * sizeof(float);
    OSPData vertices =
        ospNewData(trianglesMesh.vertices.size(), OSP_FLOAT3,
                   trianglesMesh.vertices.data(), _memoryManagementFlags);

    if (!trianglesMesh.normals.empty())
    {
        size += trianglesMesh.normals.size() * 3 * sizeof(float);
        OSPData normals =
            ospNewData(trianglesMesh.normals.size(), OSP_FLOAT3,
                       trianglesMesh.normals.data(), _memoryManagementFlags);
        ospSetObject(_ospMeshes[materialId], "vertex.normal", normals);
    }

    size += trianglesMesh.indices.size() * 3 * sizeof(int);
    OSPData indices =
        ospNewData(trianglesMesh.indices.size(), OSP_INT3,
                   trianglesMesh.indices.data(), _memoryManagementFlags);

    if (!trianglesMesh.colors.empty())
    {
        size += trianglesMesh.colors.size() * 4 * sizeof(float);
        OSPData colors =
            ospNewData(trianglesMesh.colors.size(), OSP_FLOAT3A,
                       trianglesMesh.colors.data(), _memoryManagementFlags);
        ospSetObject(_ospMeshes[materialId], "vertex.color", colors);
        ospRelease(colors);
    }

    if (!trianglesMesh.textureCoordinates.empty())
    {
        size += trianglesMesh.textureCoordinates.size() * 2 * sizeof(float);
        OSPData texCoords =
            ospNewData(trianglesMesh.textureCoordinates.size(), OSP_FLOAT2,
                       trianglesMesh.textureCoordinates.data(),
                       _memoryManagementFlags);
        ospSetObject(_ospMeshes[materialId], "vertex.texcoord", texCoords);
        ospRelease(texCoords);
    }

    ospSetObject(_ospMeshes[materialId], "position", vertices);
    ospRelease(vertices);
    ospSetObject(_ospMeshes[materialId], "index", indices);
    ospRelease(indices);
    ospSet1i(_ospMeshes[materialId], "alpha_type", 0);
    ospSet1i(_ospMeshes[materialId], "alpha_component", 4);

    auto impl =
        std::static_pointer_cast<OSPRayMaterialManager>(_materialManager);
    const auto& ospMaterial = impl->getOSPMaterial(materialId);
    ospSetMaterial(_ospMeshes[materialId], ospMaterial);
    ospCommit(_ospMeshes[materialId]);

    ospAddGeometry(_model, _ospMeshes[materialId]);
    _updateValue(_trianglesMeshesDirty, false);
    return size;
}

uint64_t OSPRayGeometryGroup::commit()
{
    if (!dirty())
        return 0;

    BRAYNS_FCT_ENTRY

    if (!_model)
        _model = ospNewModel();

    if (!_simulationModel)
        _simulationModel = ospNewModel();

    uint64_t size{0};
    if (_spheresDirty)
        for (const auto& spheres : _spheres)
            size += _commitSpheres(spheres.first);

    if (_cylindersDirty)
        for (const auto& cylinders : _cylinders)
            size += _commitCylinders(cylinders.first);

    if (_conesDirty)
        for (const auto& cones : _cones)
            size += _commitCones(cones.first);

    if (_trianglesMeshesDirty)
        for (const auto& meshes : _trianglesMeshes)
            size += _commitMeshes(meshes.first);

    ospCommit(_model);
    return size;
}

OSPGeometry OSPRayGeometryGroup::getInstance(const Vector3f& translation,
                                             const Vector3f& rotation,
                                             const Vector3f& scale)
{
    if (_instance)
        ospRelease(_instance);

    ospcommon::affine3f transformation = ospcommon::affine3f(ospcommon::one);
    transformation *= transformation.scale({scale.x(), scale.y(), scale.z()});
    transformation *= transformation.translate(
        {translation.x(), translation.y(), translation.z()});
    if (rotation.x() != 0.f)
        transformation *= transformation.rotate({1, 0, 0}, rotation.x());
    if (rotation.y() != 0.f)
        transformation *= transformation.rotate({0, 1, 0}, rotation.y());
    if (rotation.z() != 0.f)
        transformation *= transformation.rotate({0, 0, 1}, rotation.z());

    _instance = ospNewInstance(_model, (osp::affine3f&)transformation);
    return _instance;
}

OSPGeometry OSPRayGeometryGroup::getSimulationModelInstance(
    const Vector3f& translation, const Vector3f& rotation,
    const Vector3f& scale)
{
    if (_simulationModelInstance)
        ospRelease(_simulationModelInstance);

    ospcommon::affine3f transformation = ospcommon::affine3f(ospcommon::one);
    transformation *= transformation.scale({scale.x(), scale.y(), scale.z()});
    transformation *= transformation.translate(
        {translation.x(), translation.y(), translation.z()});
    if (rotation.x() != 0.f)
        transformation *= transformation.rotate({1, 0, 0}, rotation.x());
    if (rotation.y() != 0.f)
        transformation *= transformation.rotate({0, 1, 0}, rotation.y());
    if (rotation.z() != 0.f)
        transformation *= transformation.rotate({0, 0, 1}, rotation.z());

    _simulationModelInstance =
        ospNewInstance(_simulationModel, (osp::affine3f&)transformation);
    return _simulationModelInstance;
}
}
