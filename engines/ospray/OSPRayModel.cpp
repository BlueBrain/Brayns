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

    releaseAndClearGeometry(_ospSpheres);
    releaseAndClearGeometry(_ospCylinders);
    releaseAndClearGeometry(_ospCones);
    releaseAndClearGeometry(_ospMeshes);
    releaseAndClearGeometry(_ospStreamlines);
    releaseAndClearGeometry(_ospSDFGeometries);

    ospRelease(_simulationModel);
    ospRelease(_boundingBoxModel);
    ospRelease(_model);
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

OSPGeometry& OSPRayModel::_createGeometry(GeometryMap& map,
                                          const size_t materialId,
                                          const char* name)
{
    auto& geometry = map[materialId];
    if (geometry)
    {
        ospRemoveGeometry(_model, geometry);
        ospRemoveGeometry(_simulationModel, geometry);
        ospRelease(geometry);
    }
    geometry = ospNewGeometry(name);
    auto material = static_cast<OSPRayMaterial*>(_materials[materialId].get());
    if (material && material->getOSPMaterial())
        ospSetMaterial(geometry, material->getOSPMaterial());

    return geometry;
}

void OSPRayModel::_commitSpheres(const size_t materialId)
{
    auto& geometry = _createGeometry(_ospSpheres, materialId, "spheres");

    auto data = allocateVectorData(_spheres.at(materialId), OSP_FLOAT,
                                   _memoryManagementFlags);

    ospSetObject(geometry, "spheres", data);
    ospRelease(data);

    ospSet1i(geometry, "offset_center", offsetof(Sphere, center));
    ospSet1i(geometry, "offset_radius", offsetof(Sphere, radius));
    ospSet1i(geometry, "bytes_per_sphere", sizeof(Sphere));

    ospCommit(geometry);

    if (_useSimulationModel)
        ospAddGeometry(_simulationModel, geometry);
    else if (materialId == BOUNDINGBOX_MATERIAL_ID)
        ospAddGeometry(_boundingBoxModel, geometry);
    else
        ospAddGeometry(_model, geometry);
}

void OSPRayModel::_commitCylinders(const size_t materialId)
{
    auto& geometry = _createGeometry(_ospCylinders, materialId, "cylinders");

    auto data = allocateVectorData(_cylinders.at(materialId), OSP_FLOAT,
                                   _memoryManagementFlags);
    ospSetObject(geometry, "cylinders", data);
    ospRelease(data);

    ospSet1i(geometry, "offset_v0", offsetof(Cylinder, center));
    ospSet1i(geometry, "offset_v1", offsetof(Cylinder, up));
    ospSet1i(geometry, "offset_radius", offsetof(Cylinder, radius));
    ospSet1i(geometry, "bytes_per_cylinder", sizeof(Cylinder));
    ospCommit(geometry);

    if (_useSimulationModel)
        ospAddGeometry(_simulationModel, geometry);
    else if (materialId == BOUNDINGBOX_MATERIAL_ID)
        ospAddGeometry(_boundingBoxModel, geometry);
    else
        ospAddGeometry(_model, geometry);
}

void OSPRayModel::_commitCones(const size_t materialId)
{
    auto& geometry = _createGeometry(_ospCones, materialId, "cones");
    auto data = allocateVectorData(_cones.at(materialId), OSP_FLOAT,
                                   _memoryManagementFlags);

    ospSetObject(geometry, "cones", data);
    ospRelease(data);

    // Needed by the OptiX module
    ospSet1i(geometry, "bytes_per_cone", sizeof(Cone));
    ospCommit(geometry);

    if (_useSimulationModel)
        ospAddGeometry(_simulationModel, geometry);
    else if (materialId == BOUNDINGBOX_MATERIAL_ID)
        ospAddGeometry(_boundingBoxModel, geometry);
    else
        ospAddGeometry(_model, geometry);
}

void OSPRayModel::_commitMeshes(const size_t materialId)
{
    auto& geometry = _createGeometry(_ospMeshes, materialId, "trianglemesh");
    auto& trianglesMesh = _trianglesMeshes.at(materialId);

    OSPData vertices = allocateVectorData(trianglesMesh.vertices, OSP_FLOAT3,
                                          _memoryManagementFlags);
    ospSetObject(geometry, "position", vertices);
    ospRelease(vertices);

    OSPData indices = allocateVectorData(trianglesMesh.indices, OSP_INT3,
                                         _memoryManagementFlags);
    ospSetObject(geometry, "index", indices);
    ospRelease(indices);

    if (!trianglesMesh.normals.empty())
    {
        OSPData normals = allocateVectorData(trianglesMesh.normals, OSP_FLOAT3,
                                             _memoryManagementFlags);
        ospSetObject(geometry, "vertex.normal", normals);
        ospRelease(normals);
    }

    if (!trianglesMesh.colors.empty())
    {
        OSPData colors = allocateVectorData(trianglesMesh.colors, OSP_FLOAT3A,
                                            _memoryManagementFlags);
        ospSetObject(geometry, "vertex.color", colors);
        ospRelease(colors);
    }

    if (!trianglesMesh.textureCoordinates.empty())
    {
        OSPData texCoords =
            allocateVectorData(trianglesMesh.textureCoordinates, OSP_FLOAT2,
                               _memoryManagementFlags);
        ospSetObject(geometry, "vertex.texcoord", texCoords);
        ospRelease(texCoords);
    }

    ospSet1i(geometry, "alpha_type", 0);
    ospSet1i(geometry, "alpha_component", 4);

    ospCommit(geometry);

    ospAddGeometry(_model, geometry);
}

void OSPRayModel::_commitStreamlines(const size_t materialId)
{
    auto& geometry =
        _createGeometry(_ospStreamlines, materialId, "streamlines");
    auto& data = _streamlines[materialId];

    {
        OSPData vertex =
            allocateVectorData(data.vertex, OSP_FLOAT4, _memoryManagementFlags);
        ospSetObject(geometry, "vertex", vertex);
        ospRelease(vertex);
    }
    {
        OSPData vertexColor = allocateVectorData(data.vertexColor, OSP_FLOAT4,
                                                 _memoryManagementFlags);
        ospSetObject(geometry, "vertex.color", vertexColor);
        ospRelease(vertexColor);
    }
    {
        OSPData index =
            allocateVectorData(data.indices, OSP_INT, _memoryManagementFlags);
        ospSetObject(geometry, "index", index);
        ospRelease(index);
    }

    // Since we allow custom radius per point we always smooth
    ospSet1i(geometry, "smooth", true);

    ospCommit(geometry);

    ospAddGeometry(_model, geometry);
}

void OSPRayModel::_commitSDFGeometries()
{
    auto globalData =
        allocateVectorData(_sdf.geometries, OSP_CHAR, _memoryManagementFlags);

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

    auto neighbourData = allocateVectorData(_sdf.neighboursFlat, OSP_ULONG,
                                            _memoryManagementFlags);

    for (const auto& mat : _materials)
    {
        const size_t materialId = mat.first;

        if (_sdf.geometryIndices.find(materialId) == _sdf.geometryIndices.end())
            continue;

        auto& geometry =
            _createGeometry(_ospSDFGeometries, materialId, "sdfgeometries");

        auto data = allocateVectorData(_sdf.geometryIndices[materialId],
                                       OSP_ULONG, _memoryManagementFlags);
        ospSetObject(geometry, "sdfgeometries", data);
        ospRelease(data);

        ospSetData(geometry, "neighbours", neighbourData);
        ospSetData(geometry, "geometries", globalData);

        ospCommit(geometry);

        if (_useSimulationModel)
            ospAddGeometry(_simulationModel, geometry);
        else
            ospAddGeometry(_model, geometry);
    }

    ospRelease(globalData);
    ospRelease(neighbourData);
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
    ospSet1i(_model, "dynamicScene", _bvhFlags.count(BVHFlag::dynamic));
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

void OSPRayModel::commitMaterials(const std::string& renderer)
{
    if (renderer.empty())
        throw std::runtime_error(
            "Materials cannot be instanced with an empty renderer name");
    if (_renderer != renderer)
    {
        for (auto kv : _materials)
        {
            auto& material = *kv.second;
            static_cast<OSPRayMaterial&>(material).commit(renderer);
        }
        _renderer = renderer;

        for (auto& map : {_ospSpheres, _ospCylinders, _ospCones, _ospMeshes,
                          _ospStreamlines, _ospSDFGeometries})
        {
            auto matIt = _materials.begin();
            auto geomIt = map.begin();
            while (matIt != _materials.end() && geomIt != map.end())
            {
                while (matIt->first < geomIt->first)
                    ++matIt;
                if (matIt->first != geomIt->first)
                {
                    // This shouldn't happen
                    BRAYNS_ERROR << "Material for geometry missing"
                                 << std::endl;
                    ++geomIt;
                    continue;
                }
                auto& material = static_cast<OSPRayMaterial&>(*matIt->second);
                ospSetMaterial(geomIt->second, material.getOSPMaterial());
                ospCommit(geomIt->second);
                ++geomIt;
            }
        }
    }
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
