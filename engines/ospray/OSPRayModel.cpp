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
#include <brayns/common/simulation/AbstractSimulationHandler.h>

#include <brayns/parameters/ParametersManager.h>

namespace brayns
{
namespace
{
template <typename VecT>
OSPData allocateVectorData(const std::vector<VecT>& vec,
                           const OSPDataType ospType,
                           const size_t memoryManagementFlags)
{
    const size_t totBytes = vec.size() * sizeof(decltype(vec.back()));

    if (totBytes >= INT_MAX)
        BRAYNS_INFO << "Buffer allocation (" << std::to_string(totBytes)
                    << " bytes) exceeds ispc 32-bit address space."
                    << std::endl;

    return ospNewData(totBytes / ospray::sizeOf(ospType), ospType, vec.data(),
                      memoryManagementFlags);
}

double interpolatedOpacity(const Vector2ds& controlPoints, const double x)
{
    const auto& firstPoint = controlPoints.front();
    if (x <= firstPoint.x())
        return firstPoint.y();

    for (size_t i = 1; i < controlPoints.size(); ++i)
    {
        const auto& current = controlPoints[i];
        const auto& previous = controlPoints[i - 1];
        if (x <= current.x())
        {
            const auto t = (x - previous.x()) / (current.x() - previous.x());
            return (1.0 - t) * previous.y() + t * current.y();
        }
    }

    const auto& lastPoint = controlPoints.back();
    return lastPoint.y();
}
}

OSPRayModel::OSPRayModel(AnimationParameters& animationParameters,
                         VolumeParameters& volumeParameters)
    : Model()
    , _animationParameters(animationParameters)
    , _volumeParameters(volumeParameters)
{
    _ospTransferFunction = ospNewTransferFunction("piecewise_linear");
    if (_ospTransferFunction)
        ospCommit(_ospTransferFunction);
}

OSPRayModel::~OSPRayModel()
{
    if (_setIsReadyCallback)
        _animationParameters.removeIsReadyCallback();
    ospRelease(_ospTransferFunction);
    ospRelease(_ospSimulationData);

    const auto releaseAndClearGeometry = [](auto& geometryMap) {
        for (auto geom : geometryMap)
            ospRelease(geom.second);
        geometryMap.clear();
    };

    const auto releaseModel = [](const auto& model) { ospRelease(model); };

    releaseAndClearGeometry(_ospExtendedSpheres);
    releaseAndClearGeometry(_ospExtendedSpheresData);
    releaseAndClearGeometry(_ospExtendedCylinders);
    releaseAndClearGeometry(_ospExtendedCylindersData);
    releaseAndClearGeometry(_ospExtendedCones);
    releaseAndClearGeometry(_ospExtendedConesData);
    releaseAndClearGeometry(_ospMeshes);
    releaseAndClearGeometry(_ospStreamlines);
    releaseAndClearGeometry(_ospSDFGeometryRefs);
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
    const Vector3f s(0.5f);
    const Vector3f c(0.5f);
    const float radius = 0.005f;
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
}

void OSPRayModel::_commitSpheres(const size_t materialId)
{
    const auto& spheres = _spheres[materialId];

    if (_ospExtendedSpheres.find(materialId) != _ospExtendedSpheres.end())
    {
        ospRemoveGeometry(_model, _ospExtendedSpheres[materialId]);
        ospRelease(_ospExtendedSpheres[materialId]);
        ospRelease(_ospExtendedSpheresData[materialId]);
    }

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
    {
        ospRemoveGeometry(_model, _ospExtendedCylinders[materialId]);
        ospRelease(_ospExtendedCylinders[materialId]);
        ospRelease(_ospExtendedCylindersData[materialId]);
    }

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
    {
        ospRemoveGeometry(_model, _ospExtendedCones[materialId]);
        ospRelease(_ospExtendedCones[materialId]);
        ospRelease(_ospExtendedConesData[materialId]);
    }

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
    if (_ospMeshes.find(materialId) != _ospMeshes.end())
    {
        ospRemoveGeometry(_model, _ospMeshes[materialId]);
        ospRelease(_ospMeshes[materialId]);
    }

    _ospMeshes[materialId] = ospNewGeometry("trianglemesh");

    auto& trianglesMesh = _trianglesMeshes[materialId];
    OSPData vertices = allocateVectorData(trianglesMesh.vertices, OSP_FLOAT3,
                                          _memoryManagementFlags);

    if (!trianglesMesh.normals.empty())
    {
        OSPData normals = allocateVectorData(trianglesMesh.normals, OSP_FLOAT3,
                                             _memoryManagementFlags);
        ospSetObject(_ospMeshes[materialId], "vertex.normal", normals);
        ospRelease(normals);
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

    if (_ospSDFGeometryData)
        ospRelease(_ospSDFGeometryData);
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

    if (_ospSDFNeighboursData)
        ospRelease(_ospSDFNeighboursData);
    _ospSDFNeighboursData = allocateVectorData(_sdf.neighboursFlat, OSP_ULONG,
                                               _memoryManagementFlags);

    ospCommit(_ospSDFNeighboursData);

    for (const auto& mat : _materials)
    {
        const size_t materialId = mat.first;

        if (_sdf.geometryIndices.find(materialId) == _sdf.geometryIndices.end())
            continue;

        if (_ospSDFGeometryRefs.find(materialId) != _ospSDFGeometryRefs.end())
        {
            ospRemoveGeometry(_model, _ospSDFGeometryRefs[materialId]);
            ospRemoveGeometry(_simulationModel,
                              _ospSDFGeometryRefs[materialId]);
            ospRelease(_ospSDFGeometryRefs[materialId]);
        }

        _ospSDFGeometryRefs[materialId] =
            ospNewGeometry("extendedsdfgeometries");

        if (_ospSDFGeometryRefsData[materialId])
            ospRelease(_ospSDFGeometryRefsData[materialId]);
        _ospSDFGeometryRefsData[materialId] =
            allocateVectorData(_sdf.geometryIndices[materialId], OSP_ULONG,
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

bool OSPRayModel::_commitSimulationData()
{
    if (!_simulationHandler)
        return false;

    if (!_setIsReadyCallback && !_animationParameters.hasIsReadyCallback())
    {
        auto& ap = _animationParameters;
        ap.setIsReadyCallback(
            [handler = _simulationHandler] { return handler->isReady(); });
        ap.setDt(_simulationHandler->getDt());
        ap.setUnit(_simulationHandler->getUnit());
        ap.setEnd(_simulationHandler->getNbFrames());
        _setIsReadyCallback = true;
    }

    const auto animationFrame = _animationParameters.getFrame();

    if (_ospSimulationData &&
        _simulationHandler->getCurrentFrame() == animationFrame)
    {
        return false;
    }

    auto frameData = _simulationHandler->getFrameData(animationFrame);

    if (!frameData)
        return false;

    ospRelease(_ospSimulationData);
    _ospSimulationData =
        ospNewData(_simulationHandler->getFrameSize(), OSP_FLOAT, frameData,
                   _memoryManagementFlags);
    ospCommit(_ospSimulationData);
    return true;
}

bool OSPRayModel::_commitTransferFunction()
{
    if (!_transferFunction.isModified() || !_ospTransferFunction)
        return false;

    // colors
    const auto& colors = _transferFunction.getColorMap().colors;
    OSPData colorsData = ospNewData(colors.size(), OSP_FLOAT3, colors.data());
    ospSetData(_ospTransferFunction, "colors", colorsData);
    ospRelease(colorsData);

    // opacities
    auto tfPoints = _transferFunction.getControlPoints();
    std::sort(tfPoints.begin(), tfPoints.end(),
              [](auto a, auto b) { return a.x() < b.x(); });
    floats opacities;
    opacities.reserve(tfPoints.size());
    constexpr size_t numSamples = 256u;
    constexpr double dx = 1. / (numSamples - 1);
    for (size_t i = 0; i < numSamples; ++i)
        opacities.push_back(interpolatedOpacity(tfPoints, i * dx));
    OSPData opacityData =
        ospNewData(opacities.size(), OSP_FLOAT, opacities.data());
    ospSetData(_ospTransferFunction, "opacities", opacityData);
    ospRelease(opacityData);

    ospSet2f(_ospTransferFunction, "valueRange",
             _transferFunction.getValuesRange().x(),
             _transferFunction.getValuesRange().y());
    ospCommit(_ospTransferFunction);

    _transferFunction.resetModified();
    return true;
}

void OSPRayModel::_setBVHFlags()
{
    ospSet1i(_model, "dynamicMode", _bvhFlags.count(BVHFlag::dynamic));
    ospSet1i(_model, "compactMode", _bvhFlags.count(BVHFlag::compact));
    ospSet1i(_model, "robustMode", _bvhFlags.count(BVHFlag::robust));
}

void OSPRayModel::commitGeometry()
{
    for (auto volume : _volumes)
    {
        auto ospVolume = std::dynamic_pointer_cast<OSPRayVolume>(volume);
        ospVolume->commit();
    }

    if (!isDirty())
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
    }

    if (_cylindersDirty)
    {
        for (const auto& cylinders : _cylinders)
            _commitCylinders(cylinders.first);
    }

    if (_conesDirty)
    {
        for (const auto& cones : _cones)
            _commitCones(cones.first);
    }

    if (_trianglesMeshesDirty)
    {
        for (const auto& meshes : _trianglesMeshes)
            _commitMeshes(meshes.first);
    }

    if (_streamlinesDirty)
    {
        for (const auto& streamlines : _streamlines)
            _commitStreamlines(streamlines.first);
    }

    if (_sdfGeometriesDirty)
        _commitSDFGeometries();

    _updateBounds();
    _setBVHFlags();

    // handled by the scene
    _instancesDirty = false;

    // Commit models
    ospCommit(_model);
    if (_boundingBoxModel)
        ospCommit(_boundingBoxModel);
    ospCommit(_simulationModel);
}

bool OSPRayModel::commitTransferFunction()
{
    const auto dirtyTransferFunction = _commitTransferFunction();
    const auto dirtySimulationData = _commitSimulationData();
    return dirtyTransferFunction || dirtySimulationData;
}

MaterialPtr OSPRayModel::createMaterial(const size_t materialId,
                                        const std::string& name)
{
    MaterialPtr material = std::make_shared<OSPRayMaterial>();
    material->setName(name);
    _materials[materialId] = material;
    return material;
}

SharedDataVolumePtr OSPRayModel::createSharedDataVolume(
    const Vector3ui& dimensions, const Vector3f& spacing,
    const DataType type) const
{
    return std::make_shared<OSPRaySharedDataVolume>(dimensions, spacing, type,
                                                    _volumeParameters,
                                                    _ospTransferFunction);
}

BrickedVolumePtr OSPRayModel::createBrickedVolume(const Vector3ui& dimensions,
                                                  const Vector3f& spacing,
                                                  const DataType type) const
{
    return std::make_shared<OSPRayBrickedVolume>(dimensions, spacing, type,
                                                 _volumeParameters,
                                                 _ospTransferFunction);
}
}
