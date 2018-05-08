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

#include "OSPRayModel.h"
#include "OSPRayMaterial.h"

#include <brayns/common/Transformation.h>
#include <brayns/common/material/Material.h>
#include <brayns/common/scene/Scene.h>

#include <ospray/SDK/common/OSPCommon.h>

namespace brayns
{
OSPRayModel::~OSPRayModel()
{
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

    if (_simulationModelInstance)
        ospRelease(_simulationModelInstance);

    if (_simulationModel)
        ospRelease(_simulationModel);

    if (_boundingBoxModelInstance)
        ospRelease(_boundingBoxModelInstance);

    if (_boundingBoxModel)
        ospRelease(_boundingBoxModel);

    for (auto& i : _instances)
        ospRelease(i.second);

    if (_model)
        ospRelease(_model);
}

void OSPRayModel::setMemoryFlags(const size_t memoryManagementFlags)
{
    _memoryManagementFlags = memoryManagementFlags;
}

void OSPRayModel::_buildBoundingBox()
{
    _boundingBoxModel = ospNewModel();

    auto material = createMaterial(BOUNDINGBOX_MATERIAL_ID, "bounding_box");
    material->setDiffuseColor({1, 1, 1});
    material->setEmission(1.f);
    material->setCastSimulationData(false);
    material->commit();
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

    ospCommit(_boundingBoxModel);
}

void OSPRayModel::_commitSpheres(const size_t materialId)
{
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
        std::static_pointer_cast<OSPRayMaterial>(_materials[materialId]);
    ospSetMaterial(_ospExtendedSpheres[materialId], impl->getOSPMaterial());
    ospCommit(_ospExtendedSpheres[materialId]);

    if (_useSimulationModel)
        ospAddGeometry(_simulationModel, _ospExtendedSpheres[materialId]);
    else if (materialId == BOUNDINGBOX_MATERIAL_ID)
        ospAddGeometry(_boundingBoxModel, _ospExtendedSpheres[materialId]);
    else
        ospAddGeometry(_model, _ospExtendedSpheres[materialId]);
}

void OSPRayModel::_commitCylinders(const size_t materialId)
{
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
        std::static_pointer_cast<OSPRayMaterial>(_materials[materialId]);
    ospSetMaterial(_ospExtendedCylinders[materialId], impl->getOSPMaterial());

    ospCommit(_ospExtendedCylinders[materialId]);

    if (_useSimulationModel)
        ospAddGeometry(_simulationModel, _ospExtendedCylinders[materialId]);
    else if (materialId == BOUNDINGBOX_MATERIAL_ID)
        ospAddGeometry(_boundingBoxModel, _ospExtendedCylinders[materialId]);
    else
        ospAddGeometry(_model, _ospExtendedCylinders[materialId]);
}

void OSPRayModel::_commitCones(const size_t materialId)
{
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
        std::static_pointer_cast<OSPRayMaterial>(_materials[materialId]);
    ospSetMaterial(_ospExtendedCones[materialId], impl->getOSPMaterial());
    ospCommit(_ospExtendedCones[materialId]);

    if (_useSimulationModel)
        ospAddGeometry(_simulationModel, _ospExtendedCones[materialId]);
    else if (materialId == BOUNDINGBOX_MATERIAL_ID)
        ospAddGeometry(_boundingBoxModel, _ospExtendedCones[materialId]);
    else
        ospAddGeometry(_model, _ospExtendedCones[materialId]);
}

void OSPRayModel::_commitMeshes(const size_t materialId)
{
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
        std::static_pointer_cast<OSPRayMaterial>(_materials[materialId]);
    ospSetMaterial(_ospMeshes[materialId], impl->getOSPMaterial());
    ospCommit(_ospMeshes[materialId]);

    ospAddGeometry(_model, _ospMeshes[materialId]);
}

void OSPRayModel::commit()
{
    if (!dirty())
        return;

    if (!_model)
        _model = ospNewModel();

    if (!_simulationModel)
        _simulationModel = ospNewModel();

    // Materials
    for (auto material : _materials)
        material.second->commit();

    // Bounding box
    if (!_boundingBoxModel)
        _buildBoundingBox();

    // Group geometry
    if (_spheresDirty)
    {
        for (const auto& spheres : _spheres)
            _commitSpheres(spheres.first);
        _spheresDirty = false;
    }

    if (_cylindersDirty)
    {
        for (const auto& cylinders : _cylinders)
            _commitCylinders(cylinders.first);
        _cylindersDirty = false;
    }

    if (_conesDirty)
    {
        for (const auto& cones : _cones)
            _commitCones(cones.first);
        _conesDirty = false;
    }

    if (_trianglesMeshesDirty)
    {
        for (const auto& meshes : _trianglesMeshes)
            _commitMeshes(meshes.first);
        _trianglesMeshesDirty = false;
    }

    // Commit models
    ospCommit(_model);
    ospCommit(_boundingBoxModel);
    ospCommit(_simulationModel);
}

osp::affine3f OSPRayModel::_groupTransformationToAffine3f(
    const Transformation& transformation) const
{
    ospcommon::affine3f t = ospcommon::affine3f(ospcommon::one);
    const auto& scale = transformation.getScale();
    t *= t.scale({scale.x(), scale.y(), scale.z()});
    const auto& translation = transformation.getTranslation();
    t *= t.translate({translation.x(), translation.y(), translation.z()});

    const auto& matrix = transformation.getRotation().getRotationMatrix();
    const float x = atan2(matrix(2, 1), matrix(2, 2));
    const float y = atan2(-matrix(2, 0),
                          sqrt(powf(matrix(2, 1), 2) + powf(matrix(2, 2), 2)));
    const float z = atan2(matrix(1, 0), matrix(0, 0));
    if (x != 0.f)
        t *= t.rotate({1, 0, 0}, x);
    if (y != 0.f)
        t *= t.rotate({0, 1, 0}, y);
    if (z != 0.f)
        t *= t.rotate({0, 0, 1}, z);
    return (osp::affine3f&)t;
}

OSPGeometry OSPRayModel::getInstance(const size_t index,
                                     const Transformation& transformation)
{
    auto it = _instances.find(index);
    if (it != _instances.end())
    {
        ospRelease(it->second);
        _instances.erase(it);
    }

    OSPGeometry instance =
        ospNewInstance(_model, _groupTransformationToAffine3f(transformation));
    ospCommit(instance);
    _instances[index] = instance;
    return instance;
}

OSPGeometry OSPRayModel::getSimulationModelInstance(
    const Transformation& transformation)
{
    if (_simulationModelInstance)
        ospRelease(_simulationModelInstance);

    _simulationModelInstance =
        ospNewInstance(_simulationModel,
                       _groupTransformationToAffine3f(transformation));
    ospCommit(_simulationModelInstance);
    return _simulationModelInstance;
}

OSPGeometry OSPRayModel::getBoundingBoxModelInstance(
    const Transformation& transformation)
{
    if (_boundingBoxModelInstance)
        ospRelease(_boundingBoxModelInstance);

    _boundingBoxModelInstance =
        ospNewInstance(_boundingBoxModel,
                       _groupTransformationToAffine3f(transformation));
    ospCommit(_boundingBoxModelInstance);
    return _boundingBoxModelInstance;
}

MaterialPtr OSPRayModel::createMaterial(const size_t materialId,
                                        const std::string& name)
{
    MaterialPtr material = std::make_shared<OSPRayMaterial>();
    material->setName(name);
    _materials[materialId] = material;
    return material;
}
}
