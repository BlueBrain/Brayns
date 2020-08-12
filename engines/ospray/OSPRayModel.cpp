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

#include <brayns/common/simulation/AbstractSimulationHandler.h>
#include <brayns/engineapi/Material.h>
#include <brayns/engineapi/Scene.h>
#include <brayns/parameters/AnimationParameters.h>

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
} // namespace

OSPRayModel::OSPRayModel(AnimationParameters& animationParameters,
                         VolumeParameters& volumeParameters)
    : Model(animationParameters, volumeParameters)
{
    _ospTransferFunction = ospNewTransferFunction("piecewise_linear");
    if (_ospTransferFunction)
        ospCommit(_ospTransferFunction);
}

OSPRayModel::~OSPRayModel()
{
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

    ospRelease(_primaryModel);
    ospRelease(_secondaryModel);
    ospRelease(_boundingBoxModel);
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

void OSPRayModel::_addGeometryToModel(const OSPGeometry geometry,
                                      const size_t materialId)
{
    switch (materialId)
    {
    case BOUNDINGBOX_MATERIAL_ID:
        ospAddGeometry(_boundingBoxModel, geometry);
        break;
    case SECONDARY_MODEL_MATERIAL_ID:
    {
        if (!_secondaryModel)
            _secondaryModel = ospNewModel();
        ospAddGeometry(_secondaryModel, geometry);
        break;
    }
    default:
        ospAddGeometry(_primaryModel, geometry);
    }
}

OSPGeometry& OSPRayModel::_createGeometry(GeometryMap& map,
                                          const size_t materialId,
                                          const char* name)
{
    auto& geometry = map[materialId];
    if (geometry)
    {
        ospRemoveGeometry(_primaryModel, geometry);
        ospRelease(geometry);
    }
    geometry = ospNewGeometry(name);

    auto matIt = _materials.find(materialId);
    if (matIt != _materials.end())
    {
        auto material = std::static_pointer_cast<OSPRayMaterial>(matIt->second);
        if (material->getOSPMaterial())
            ospSetMaterial(geometry, material->getOSPMaterial());
    }

    return geometry;
}

void OSPRayModel::_commitSpheres(const size_t materialId)
{
    auto& geometry = _createGeometry(_ospSpheres, materialId, "spheres");

    auto data = allocateVectorData(_geometries->_spheres.at(materialId),
                                   OSP_FLOAT, _memoryManagementFlags);

    ospSetObject(geometry, "spheres", data);
    ospRelease(data);

    osphelper::set(geometry, "offset_center",
                   static_cast<int>(offsetof(Sphere, center)));
    osphelper::set(geometry, "offset_radius",
                   static_cast<int>(offsetof(Sphere, radius)));
    osphelper::set(geometry, "bytes_per_sphere",
                   static_cast<int>(sizeof(Sphere)));
    ospCommit(geometry);

    _addGeometryToModel(geometry, materialId);
}

void OSPRayModel::_commitCylinders(const size_t materialId)
{
    auto& geometry = _createGeometry(_ospCylinders, materialId, "cylinders");

    auto data = allocateVectorData(_geometries->_cylinders.at(materialId),
                                   OSP_FLOAT, _memoryManagementFlags);
    ospSetObject(geometry, "cylinders", data);
    ospRelease(data);

    osphelper::set(geometry, "offset_v0",
                   static_cast<int>(offsetof(Cylinder, center)));
    osphelper::set(geometry, "offset_v1",
                   static_cast<int>(offsetof(Cylinder, up)));
    osphelper::set(geometry, "offset_radius",
                   static_cast<int>(offsetof(Cylinder, radius)));
    osphelper::set(geometry, "bytes_per_cylinder",
                   static_cast<int>(sizeof(Cylinder)));
    ospCommit(geometry);

    _addGeometryToModel(geometry, materialId);
}

void OSPRayModel::_commitCones(const size_t materialId)
{
    auto& geometry = _createGeometry(_ospCones, materialId, "cones");
    auto data = allocateVectorData(_geometries->_cones.at(materialId),
                                   OSP_FLOAT, _memoryManagementFlags);

    ospSetObject(geometry, "cones", data);
    ospRelease(data);

    ospCommit(geometry);

    _addGeometryToModel(geometry, materialId);
}

void OSPRayModel::_commitSDFBeziers(const size_t materialId)
{
    auto& geometry = _createGeometry(_ospSDFBeziers, materialId, "sdfbeziers");
    auto data = allocateVectorData(_geometries->_sdfBeziers.at(materialId),
                                   OSP_FLOAT, _memoryManagementFlags);

    ospSetObject(geometry, "sdfbeziers", data);
    ospRelease(data);

    ospCommit(geometry);

    _addGeometryToModel(geometry, materialId);
}

void OSPRayModel::_commitMeshes(const size_t materialId)
{
    auto& geometry = _createGeometry(_ospMeshes, materialId, "trianglemesh");
    auto& triangleMesh = _geometries->_triangleMeshes.at(materialId);

    OSPData vertices = allocateVectorData(triangleMesh.vertices, OSP_FLOAT3,
                                          _memoryManagementFlags);
    ospSetObject(geometry, "position", vertices);
    ospRelease(vertices);

    OSPData indices = allocateVectorData(triangleMesh.indices, OSP_INT3,
                                         _memoryManagementFlags);
    ospSetObject(geometry, "index", indices);
    ospRelease(indices);

    if (!triangleMesh.normals.empty())
    {
        OSPData normals = allocateVectorData(triangleMesh.normals, OSP_FLOAT3,
                                             _memoryManagementFlags);
        ospSetObject(geometry, "vertex.normal", normals);
        ospRelease(normals);
    }

    if (!triangleMesh.colors.empty())
    {
        OSPData colors = allocateVectorData(triangleMesh.colors, OSP_FLOAT3A,
                                            _memoryManagementFlags);
        ospSetObject(geometry, "vertex.color", colors);
        ospRelease(colors);
    }

    if (!triangleMesh.textureCoordinates.empty())
    {
        OSPData texCoords =
            allocateVectorData(triangleMesh.textureCoordinates, OSP_FLOAT2,
                               _memoryManagementFlags);
        ospSetObject(geometry, "vertex.texcoord", texCoords);
        ospRelease(texCoords);
    }

    osphelper::set(geometry, "alpha_type", 0);
    osphelper::set(geometry, "alpha_component", 4);

    ospCommit(geometry);

    ospAddGeometry(_primaryModel, geometry);
}

void OSPRayModel::_commitStreamlines(const size_t materialId)
{
    auto& geometry =
        _createGeometry(_ospStreamlines, materialId, "streamlines");
    auto& data = _geometries->_streamlines[materialId];

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
    osphelper::set(geometry, "smooth", true);

    ospCommit(geometry);

    ospAddGeometry(_primaryModel, geometry);
}

void OSPRayModel::_commitSDFGeometries()
{
    auto globalData = allocateVectorData(_geometries->_sdf.geometries, OSP_CHAR,
                                         _memoryManagementFlags);

    // Create and upload flat list of neighbours
    const size_t numGeoms = _geometries->_sdf.geometries.size();
    _geometries->_sdf.neighboursFlat.clear();

    for (size_t geomI = 0; geomI < numGeoms; geomI++)
    {
        const size_t currOffset = _geometries->_sdf.neighboursFlat.size();
        const auto& neighsI = _geometries->_sdf.neighbours[geomI];
        if (!neighsI.empty())
        {
            _geometries->_sdf.geometries[geomI].numNeighbours = neighsI.size();
            _geometries->_sdf.geometries[geomI].neighboursIndex = currOffset;
            _geometries->_sdf.neighboursFlat.insert(
                std::end(_geometries->_sdf.neighboursFlat), std::begin(neighsI),
                std::end(neighsI));
        }
    }

    // Make sure we don't create an empty buffer in the case of no neighbours
    if (_geometries->_sdf.neighboursFlat.empty())
        _geometries->_sdf.neighboursFlat.resize(1, 0);

    auto neighbourData = allocateVectorData(_geometries->_sdf.neighboursFlat,
                                            OSP_ULONG, _memoryManagementFlags);

    for (const auto& mat : _materials)
    {
        const size_t materialId = mat.first;

        if (_geometries->_sdf.geometryIndices.find(materialId) ==
            _geometries->_sdf.geometryIndices.end())
            continue;

        auto& geometry =
            _createGeometry(_ospSDFGeometries, materialId, "sdfgeometries");

        auto data =
            allocateVectorData(_geometries->_sdf.geometryIndices[materialId],
                               OSP_ULONG, _memoryManagementFlags);
        ospSetObject(geometry, "sdfgeometries", data);
        ospRelease(data);

        ospSetData(geometry, "neighbours", neighbourData);
        ospSetData(geometry, "geometries", globalData);

        ospCommit(geometry);

        ospAddGeometry(_primaryModel, geometry);
    }

    ospRelease(globalData);
    ospRelease(neighbourData);
}

void OSPRayModel::_setBVHFlags()
{
    osphelper::set(_primaryModel, "dynamicScene",
                   static_cast<int>(_bvhFlags.count(BVHFlag::dynamic)));
    osphelper::set(_primaryModel, "compactMode",
                   static_cast<int>(_bvhFlags.count(BVHFlag::compact)));
    osphelper::set(_primaryModel, "robustMode",
                   static_cast<int>(_bvhFlags.count(BVHFlag::robust)));
}

void OSPRayModel::commitGeometry()
{
    for (auto volume : _geometries->_volumes)
    {
        auto ospVolume = std::dynamic_pointer_cast<OSPRayVolume>(volume);
        ospVolume->commit();
    }

    if (!isDirty())
        return;

    if (!_primaryModel)
        _primaryModel = ospNewModel();

    // Materials
    for (auto material : _materials)
        material.second->commit();

    // Group geometry
    if (_spheresDirty)
    {
        for (const auto& spheres : _geometries->_spheres)
            _commitSpheres(spheres.first);
    }

    if (_cylindersDirty)
    {
        for (const auto& cylinders : _geometries->_cylinders)
            _commitCylinders(cylinders.first);
    }

    if (_conesDirty)
    {
        for (const auto& cones : _geometries->_cones)
            _commitCones(cones.first);
    }

    if (_sdfBeziersDirty)
    {
        for (const auto& sdfBeziers : _geometries->_sdfBeziers)
            _commitSDFBeziers(sdfBeziers.first);
    }

    if (_triangleMeshesDirty)
    {
        for (const auto& meshes : _geometries->_triangleMeshes)
            _commitMeshes(meshes.first);
    }

    if (_streamlinesDirty)
    {
        for (const auto& streamlines : _geometries->_streamlines)
            _commitStreamlines(streamlines.first);
    }

    if (_sdfGeometriesDirty)
        _commitSDFGeometries();

    updateBounds();
    _markGeometriesClean();
    _setBVHFlags();

    // handled by the scene
    _instancesDirty = false;

    // Commit models
    ospCommit(_primaryModel);
    if (_secondaryModel)
        ospCommit(_secondaryModel);
    if (_boundingBoxModel)
        ospCommit(_boundingBoxModel);
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
                while (matIt->first < geomIt->first &&
                       matIt != _materials.end())
                    ++matIt;
                if (matIt->first != geomIt->first)
                {
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
    else
    {
        for (auto kv : _materials)
        {
            auto& material = *kv.second;
            static_cast<OSPRayMaterial&>(material).commit();
        }
    }
}

MaterialPtr OSPRayModel::createMaterialImpl(const PropertyMap& properties)
{
    return std::make_shared<OSPRayMaterial>(properties);
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

void OSPRayModel::_commitTransferFunctionImpl(const Vector3fs& colors,
                                              const floats& opacities,
                                              const Vector2d valueRange)
{
    // Colors
    OSPData colorsData = ospNewData(colors.size(), OSP_FLOAT3, colors.data());
    ospSetData(_ospTransferFunction, "colors", colorsData);
    ospRelease(colorsData);

    // Opacities
    OSPData opacityData =
        ospNewData(opacities.size(), OSP_FLOAT, opacities.data());
    ospSetData(_ospTransferFunction, "opacities", opacityData);
    ospRelease(opacityData);

    // Value range
    osphelper::set(_ospTransferFunction, "valueRange", Vector2f(valueRange));

    ospCommit(_ospTransferFunction);
}

void OSPRayModel::_commitSimulationDataImpl(const float* frameData,
                                            const size_t frameSize)
{
    ospRelease(_ospSimulationData);
    _ospSimulationData =
        ospNewData(frameSize, OSP_FLOAT, frameData, _memoryManagementFlags);
    ospCommit(_ospSimulationData);
}
} // namespace brayns
