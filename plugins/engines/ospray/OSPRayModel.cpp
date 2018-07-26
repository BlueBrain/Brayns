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
#include "OSPRayVolume.h"
#include "utils.h"

#include <brayns/common/material/Material.h>
#include <brayns/common/scene/Scene.h>

namespace
{
template <typename VecT>
OSPData allocateVectorData(const std::vector<VecT>& vec,
                           const OSPDataType ospType,
                           const size_t memoryManagementFlags)
{
    const size_t totBytes = vec.size() * sizeof(decltype(vec.back()));

    if (totBytes >= INT_MAX)
        throw std::runtime_error("Buffer allocation (" +
                                 std::to_string(totBytes) + " bytes) too big.");

    return ospNewData(totBytes / ospray::sizeOf(ospType), ospType, vec.data(),
                      memoryManagementFlags);
}
}

namespace brayns
{
OSPRayModel::~OSPRayModel()
{
    const auto releaseAndClearGeometry = [](auto& geometryMap) {
        for (auto geom : geometryMap)
            ospRelease(geom.second);
        geometryMap.clear();
    };

    const auto releaseModel = [](const auto& model) {
        if (model)
            ospRelease(model);
    };

    releaseAndClearGeometry(_ospExtendedSpheres);
    releaseAndClearGeometry(_ospExtendedSpheresData);
    releaseAndClearGeometry(_ospExtendedCylinders);
    releaseAndClearGeometry(_ospExtendedCylindersData);
    releaseAndClearGeometry(_ospExtendedCones);
    releaseAndClearGeometry(_ospExtendedConesData);
    releaseAndClearGeometry(_ospMeshes);
    releaseAndClearGeometry(_ospStreamlines);
    releaseAndClearGeometry(_ospSDFGeometryRefsData);

    releaseModel(_simulationModel);
    releaseModel(_boundingBoxModel);
    releaseModel(_ospSDFGeometryData);
    releaseModel(_ospSDFNeighboursData);
    releaseModel(_model);
}

void OSPRayModel::setMemoryFlags(const size_t memoryManagementFlags)
{
    _memoryManagementFlags = memoryManagementFlags;
}

void OSPRayModel::buildBoundingBox()
{
    if (_boundingBoxModel)
        return;
    _boundingBoxModel = ospNewModel();

    auto material = createMaterial(BOUNDINGBOX_MATERIAL_ID, "bounding_box");
    material->setDiffuseColor({1, 1, 1});
    material->setEmission(1.f);
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

    if (_ospExtendedSpheres.find(materialId) != _ospExtendedSpheres.end())
        ospRemoveGeometry(_model, _ospExtendedSpheres[materialId]);

    _ospExtendedSpheres[materialId] = ospNewGeometry("extendedspheres");
    _ospExtendedSpheresData[materialId] =
        allocateVectorData(spheres, OSP_FLOAT, _memoryManagementFlags);

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
    if (_ospExtendedCylinders.find(materialId) != _ospExtendedCylinders.end())
        ospRemoveGeometry(_model, _ospExtendedCylinders[materialId]);

    _ospExtendedCylinders[materialId] = ospNewGeometry("extendedcylinders");
    _ospExtendedCylindersData[materialId] =
        allocateVectorData(cylinders, OSP_FLOAT, _memoryManagementFlags);
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
    if (_ospExtendedCones.find(materialId) != _ospExtendedCones.end())
        ospRemoveGeometry(_model, _ospExtendedCones[materialId]);

    _ospExtendedCones[materialId] = ospNewGeometry("extendedcones");
    _ospExtendedConesData[materialId] =
        allocateVectorData(cones, OSP_FLOAT, _memoryManagementFlags);

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
    OSPData vertices = allocateVectorData(trianglesMesh.vertices, OSP_FLOAT3,
                                          _memoryManagementFlags);

    if (!trianglesMesh.normals.empty())
    {
        OSPData normals = allocateVectorData(trianglesMesh.normals, OSP_FLOAT3,
                                             _memoryManagementFlags);
        ospSetObject(_ospMeshes[materialId], "vertex.normal", normals);
    }

    OSPData indices = allocateVectorData(trianglesMesh.indices, OSP_INT3,
                                         _memoryManagementFlags);

    if (!trianglesMesh.colors.empty())
    {
        OSPData colors = allocateVectorData(trianglesMesh.colors, OSP_FLOAT3A,
                                            _memoryManagementFlags);
        ospSetObject(_ospMeshes[materialId], "vertex.color", colors);
        ospRelease(colors);
    }

    if (!trianglesMesh.textureCoordinates.empty())
    {
        OSPData texCoords =
            allocateVectorData(trianglesMesh.textureCoordinates, OSP_FLOAT2,
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

void OSPRayModel::_commitStreamlines(const size_t materialId)
{
    auto streamlineGeometry = ospNewGeometry("streamlines");
    auto& streamlinesData = _streamlines[materialId];

    {
        OSPData vertex = allocateVectorData(streamlinesData.vertex, OSP_FLOAT4,
                                            _memoryManagementFlags);
        ospSetObject(streamlineGeometry, "vertex", vertex);
        ospRelease(vertex);
    }
    {
        OSPData vertexColor =
            allocateVectorData(streamlinesData.vertexColor, OSP_FLOAT4,
                               _memoryManagementFlags);
        ospSetObject(streamlineGeometry, "vertex.color", vertexColor);
        ospRelease(vertexColor);
    }
    {
        OSPData index = allocateVectorData(streamlinesData.indices, OSP_INT,
                                           _memoryManagementFlags);
        ospSetObject(streamlineGeometry, "index", index);
        ospRelease(index);
    }

    // Since we allow custom radius per point we always smooth
    ospSet1i(streamlineGeometry, "smooth", true);

    auto impl =
        std::static_pointer_cast<OSPRayMaterial>(_materials[materialId]);
    ospSetMaterial(streamlineGeometry, impl->getOSPMaterial());
    ospCommit(streamlineGeometry);

    ospAddGeometry(_model, streamlineGeometry);
    _ospStreamlines[materialId] = streamlineGeometry;
}

void OSPRayModel::_commitSDFGeometries()
{
    assert(_ospSDFGeometryData == nullptr);
    assert(_ospSDFNeighboursData == nullptr);

    _ospSDFGeometryData =
        allocateVectorData(_sdf.geometries, OSP_CHAR, _memoryManagementFlags);
    ospCommit(_ospSDFGeometryData);

    // Create and upload flat list of neighbours
    const size_t numGeoms = _sdf.geometries.size();
    _sdf.neighboursFlat.clear();

    for (size_t geomI = 0; geomI < numGeoms; geomI++)
    {
        const size_t currOffset = _sdf.neighboursFlat.size();
        const auto& neighsI = _sdf.neighbours[geomI];
        if (!neighsI.empty())
        {
            _sdf.geometries[geomI].numNeighbours = neighsI.size();
            _sdf.geometries[geomI].neighboursIndex = currOffset;
            _sdf.neighboursFlat.insert(std::end(_sdf.neighboursFlat),
                                       std::begin(neighsI), std::end(neighsI));
        }
    }

    // Make sure we don't create an empty buffer in the case of no neighbours
    if (_sdf.neighboursFlat.empty())
        _sdf.neighboursFlat.resize(1, 0);

    _ospSDFNeighboursData = allocateVectorData(_sdf.neighboursFlat, OSP_CHAR,
                                               _memoryManagementFlags);

    ospCommit(_ospSDFNeighboursData);

    for (const auto& mat : _materials)
    {
        const size_t materialId = mat.first;

        if (_sdf.geometryIndices.find(materialId) == _sdf.geometryIndices.end())
            continue;

        if (_ospSDFGeometryRefs.find(materialId) != _ospSDFGeometryRefs.end())
            ospRemoveGeometry(_model, _ospSDFGeometryRefs[materialId]);

        _ospSDFGeometryRefs[materialId] =
            ospNewGeometry("extendedsdfgeometries");

        _ospSDFGeometryRefsData[materialId] =
            allocateVectorData(_sdf.geometryIndices[materialId], OSP_UINT,
                               _memoryManagementFlags);

        ospSetObject(_ospSDFGeometryRefs[materialId], "extendedsdfgeometries",
                     _ospSDFGeometryRefsData[materialId]);

        ospSetData(_ospSDFGeometryRefs[materialId], "neighbours",
                   _ospSDFNeighboursData);

        ospSetData(_ospSDFGeometryRefs[materialId], "geometries",
                   _ospSDFGeometryData);

        if (_materials[materialId] != nullptr)
        {
            auto impl = std::static_pointer_cast<OSPRayMaterial>(
                _materials[materialId]);

            ospSetMaterial(_ospSDFGeometryRefs[materialId],
                           impl->getOSPMaterial());
        }

        ospCommit(_ospSDFGeometryRefs[materialId]);

        if (_useSimulationModel)
            ospAddGeometry(_simulationModel, _ospSDFGeometryRefs[materialId]);
        else
            ospAddGeometry(_model, _ospSDFGeometryRefs[materialId]);
    }
}

void OSPRayModel::commit()
{
    for (auto volume : _volumes)
    {
        auto ospVolume = std::dynamic_pointer_cast<OSPRayVolume>(volume);
        ospVolume->commit();
    }

    if (!dirty())
        return;

    if (!_model)
        _model = ospNewModel();

    if (!_simulationModel)
        _simulationModel = ospNewModel();

    // Materials
    for (auto material : _materials)
        material.second->commit();

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

    if (_streamlinesDirty)
    {
        for (const auto& streamlines : _streamlines)
            _commitStreamlines(streamlines.first);
        _streamlinesDirty = false;
    }

    if (_sdfGeometriesDirty)
    {
        _commitSDFGeometries();
        _sdfGeometriesDirty = false;
    }

    // handled by the scene
    _instancesDirty = false;

    // Commit models
    ospCommit(_model);
    if (_boundingBoxModel)
        ospCommit(_boundingBoxModel);
    ospCommit(_simulationModel);
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
