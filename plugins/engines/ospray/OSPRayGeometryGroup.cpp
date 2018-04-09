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

    _instances.push_back(nullptr);
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

    if (_boundingBoxModel)
        ospRelease(_boundingBoxModel);
    _boundingBoxModel = nullptr;

    _updateValue(_spheresDirty, false);
    _updateValue(_cylindersDirty, false);
    _updateValue(_conesDirty, false);
    _updateValue(_trianglesMeshesDirty, false);

    _instances.push_back(nullptr);
}

void OSPRayGeometryGroup::_buildBoundingBox()
{
    if (_boundingBoxModel)
        return;

    _boundingBoxModel = ospNewModel();

    _boudingBoxMaterialId = 0;
    const Vector3f s = _bounds.getSize() / 2.f;
    const Vector3f c = _bounds.getCenter();
    const float radius = s.length() / 200.f;
    const Vector3f positions[8] = {
        {c.x() - s.x(), c.y() - s.y(), c.z() - s.z()},
        {c.x() + s.x(), c.y() - s.y(), c.z() - s.z()}, //    6--------7
        {c.x() - s.x(), c.y() + s.y(), c.z() - s.z()}, //   /|       /|
        {c.x() + s.x(), c.y() + s.y(), c.z() - s.z()}, //  2--------3 |
        {c.x() - s.x(), c.y() - s.y(), c.z() + s.z()}, //  | |      | |
        {c.x() + s.x(), c.y() - s.y(), c.z() + s.z()}, //  | 4------|-5
        {c.x() - s.x(), c.y() + s.y(), c.z() + s.z()}, //  |/       |/
        {c.x() + s.x(), c.y() + s.y(), c.z() + s.z()}  //  0--------1
    };

    for (size_t i = 0; i < 8; ++i)
        addSphere(_boudingBoxMaterialId, Sphere(positions[i], radius));

    addCylinder(_boudingBoxMaterialId, {positions[0], positions[1], radius});
    addCylinder(_boudingBoxMaterialId, {positions[2], positions[3], radius});
    addCylinder(_boudingBoxMaterialId, {positions[4], positions[5], radius});
    addCylinder(_boudingBoxMaterialId, {positions[6], positions[7], radius});

    addCylinder(_boudingBoxMaterialId, {positions[0], positions[2], radius});
    addCylinder(_boudingBoxMaterialId, {positions[1], positions[3], radius});
    addCylinder(_boudingBoxMaterialId, {positions[4], positions[6], radius});
    addCylinder(_boudingBoxMaterialId, {positions[5], positions[7], radius});

    addCylinder(_boudingBoxMaterialId, {positions[0], positions[4], radius});
    addCylinder(_boudingBoxMaterialId, {positions[1], positions[5], radius});
    addCylinder(_boudingBoxMaterialId, {positions[2], positions[6], radius});
    addCylinder(_boudingBoxMaterialId, {positions[3], positions[7], radius});

    ospCommit(_boundingBoxModel);
}

void OSPRayGeometryGroup::_commitSpheres(const size_t materialId)
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
    else if (materialId == _boudingBoxMaterialId)
        ospAddGeometry(_boundingBoxModel, _ospExtendedSpheres[materialId]);
    else
        ospAddGeometry(_model, _ospExtendedSpheres[materialId]);

    _updateValue(_spheresDirty, false);
}

void OSPRayGeometryGroup::_commitCylinders(const size_t materialId)
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
    else if (materialId == _boudingBoxMaterialId)
        ospAddGeometry(_boundingBoxModel, _ospExtendedCylinders[materialId]);
    else
        ospAddGeometry(_model, _ospExtendedCylinders[materialId]);
    _updateValue(_cylindersDirty, false);
}

void OSPRayGeometryGroup::_commitCones(const size_t materialId)
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
    else if (materialId == _boudingBoxMaterialId)
        ospAddGeometry(_boundingBoxModel, _ospExtendedCones[materialId]);
    else
        ospAddGeometry(_model, _ospExtendedCones[materialId]);
    _updateValue(_conesDirty, false);
}

void OSPRayGeometryGroup::_commitMeshes(const size_t materialId)
{
    BRAYNS_FCT_ENTRY
    _ospMeshes[materialId] = ospNewGeometry("trianglemesh");

    auto& trianglesMesh = _trianglesMeshes[materialId];
    OSPData vertices =
        ospNewData(trianglesMesh.vertices.size(), OSP_FLOAT3,
                   trianglesMesh.vertices.data(), _memoryManagementFlags);

    if (!trianglesMesh.normals.empty())
    {
        OSPData normals =
            ospNewData(trianglesMesh.normals.size(), OSP_FLOAT3,
                       trianglesMesh.normals.data(), _memoryManagementFlags);
        ospSetObject(_ospMeshes[materialId], "vertex.normal", normals);
    }

    OSPData indices =
        ospNewData(trianglesMesh.indices.size(), OSP_INT3,
                   trianglesMesh.indices.data(), _memoryManagementFlags);

    if (!trianglesMesh.colors.empty())
    {
        OSPData colors =
            ospNewData(trianglesMesh.colors.size(), OSP_FLOAT3A,
                       trianglesMesh.colors.data(), _memoryManagementFlags);
        ospSetObject(_ospMeshes[materialId], "vertex.color", colors);
        ospRelease(colors);
    }

    if (!trianglesMesh.textureCoordinates.empty())
    {
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
}

void OSPRayGeometryGroup::commit()
{
    if (!dirty())
        return;

    BRAYNS_FCT_ENTRY

    if (!_model)
        _model = ospNewModel();

    if (!_simulationModel)
        _simulationModel = ospNewModel();

    if (_spheresDirty)
        for (const auto& spheres : _spheres)
            _commitSpheres(spheres.first);

    if (_cylindersDirty)
        for (const auto& cylinders : _cylinders)
            _commitCylinders(cylinders.first);

    if (_conesDirty)
        for (const auto& cones : _cones)
            _commitCones(cones.first);

    if (_trianglesMeshesDirty)
        for (const auto& meshes : _trianglesMeshes)
            _commitMeshes(meshes.first);

    // Bounding box
    _buildBoundingBox();

    // Commit models
    ospCommit(_model);
    ospCommit(_boundingBoxModel);
    ospCommit(_simulationModel);
    return;
}

OSPGeometry OSPRayGeometryGroup::getInstance(const size_t index,
                                             const Vector3f& translation,
                                             const Vector3f& rotation,
                                             const Vector3f& scale)
{
    if (index < _instances.size())
        _instances.push_back(nullptr);

    if (_instances[index])
        ospRelease(_instances[index]);

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

    _instances[index] = ospNewInstance(_model, (osp::affine3f&)transformation);
    ospCommit(_instances[index]);
    return _instances[index];
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
    ospCommit(_simulationModelInstance);
    return _simulationModelInstance;
}

OSPGeometry OSPRayGeometryGroup::getBoundingBoxModelInstance(
    const Vector3f& translation, const Vector3f& rotation,
    const Vector3f& scale)
{
    if (_boundingBoxModelInstance)
        ospRelease(_boundingBoxModelInstance);

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

    _boundingBoxModelInstance =
        ospNewInstance(_boundingBoxModel, (osp::affine3f&)transformation);
    ospCommit(_boundingBoxModelInstance);
    return _boundingBoxModelInstance;
}
}
