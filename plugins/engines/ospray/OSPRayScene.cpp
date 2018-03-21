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

#include "OSPRayScene.h"
#include "OSPRayRenderer.h"

#include <brayns/common/geometry/GeometryGroup.h>
#include <brayns/common/light/DirectionalLight.h>
#include <brayns/common/light/PointLight.h>
#include <brayns/common/log.h>
#include <brayns/common/material/Material.h>
#include <brayns/common/material/Texture2D.h>
#include <brayns/common/simulation/AbstractSimulationHandler.h>
#include <brayns/common/volume/VolumeHandler.h>
#include <brayns/io/ImageManager.h>
#include <brayns/parameters/GeometryParameters.h>
#include <brayns/parameters/SceneParameters.h>

#include <boost/algorithm/string/predicate.hpp> // ends_with

namespace brayns
{
struct TextureTypeMaterialAttribute
{
    TextureType type;
    std::string attribute;
};

static TextureTypeMaterialAttribute textureTypeMaterialAttribute[7] = {
    {TT_DIFFUSE, "map_kd"},
    {TT_NORMALS, "map_bump"},
    {TT_SPECULAR, "map_ks"},
    {TT_EMISSIVE, "map_a"},
    {TT_OPACITY, "map_d"},
    {TT_REFLECTION, "map_reflection"},
    {TT_REFRACTION, "map_refraction"}};

OSPRayScene::OSPRayScene(Renderers renderers,
                         ParametersManager& parametersManager)
    : Scene(renderers, parametersManager)
    , _rootModel(nullptr)
    , _simulationModel(nullptr)
    , _ospLightData(nullptr)
    , _ospMaterialData(nullptr)
    , _ospVolumeData(nullptr)
    , _ospSimulationData(nullptr)
    , _ospTransferFunctionDiffuseData(nullptr)
    , _ospTransferFunctionEmissionData(nullptr)
{
}

OSPRayScene::~OSPRayScene()
{
    if (_ospTransferFunctionDiffuseData)
        ospRelease(_ospTransferFunctionDiffuseData);

    if (_ospTransferFunctionEmissionData)
        ospRelease(_ospTransferFunctionEmissionData);

    for (auto& light : _ospLights)
        ospRelease(light);
    _ospLights.clear();
}

void OSPRayScene::unload()
{
    const auto nbMaterials = _getNbMaterials();
    if (_rootModel)
    {
        for (size_t materialId = 0; materialId < nbMaterials; ++materialId)
        {
            if (_ospMeshes[materialId])
                ospRemoveGeometry(_rootModel, _ospMeshes[materialId]);
            if (_ospExtendedSpheres[materialId])
                ospRemoveGeometry(_rootModel, _ospExtendedSpheres[materialId]);
            if (_ospExtendedCylinders[materialId])
                ospRemoveGeometry(_rootModel,
                                  _ospExtendedCylinders[materialId]);
            if (_ospExtendedCones[materialId])
                ospRemoveGeometry(_rootModel, _ospExtendedCones[materialId]);
        }
        ospCommit(_rootModel);
        ospRelease(_rootModel);
        _rootModel = nullptr;
    }

    if (_simulationModel)
    {
        for (size_t materialId = 0; materialId < nbMaterials; ++materialId)
        {
            ospRemoveGeometry(_simulationModel,
                              _ospExtendedSpheres[materialId]);
            ospRemoveGeometry(_simulationModel,
                              _ospExtendedCylinders[materialId]);
            ospRemoveGeometry(_simulationModel, _ospExtendedCones[materialId]);
        }
        ospCommit(_simulationModel);
        ospRelease(_simulationModel);
        _simulationModel = nullptr;
    }

    Scene::unload();

    for (auto& material : _ospMaterials)
        ospRelease(material);
    _ospMaterials.clear();

    for (auto& texture : _ospTextures)
        ospRelease(texture.second);
    _ospTextures.clear();

    if (_ospSimulationData)
        ospRelease(_ospSimulationData);

    if (_ospVolumeData)
        ospRelease(_ospVolumeData);

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
}

void OSPRayScene::commit()
{
    if (_simulationModel)
    {
        BRAYNS_INFO << "Committing simulation model" << std::endl;
        ospCommit(_simulationModel);
    }

    if (_rootModel)
        ospRelease(_rootModel);

    BRAYNS_INFO << "Committing root model" << std::endl;
    _rootModel = ospNewModel();

    for (size_t i = 0; i < _ospGeometryGroups.size(); ++i)
    {
        auto& group = _geometryGroups[i];
        const auto& ospGroup = _ospGeometryGroups[i];
        if (group.enabled())
        {
            if (group.dirty())
                ospCommit(ospGroup.model);

            ospcommon::affine3f transformation =
                ospcommon::affine3f(ospcommon::one);
            OSPGeometry modelInstance =
                ospNewInstance(ospGroup.model, (osp::affine3f&)transformation);
            ospAddGeometry(_rootModel, modelInstance);
        }
    }
    ospCommit(_rootModel);
}

uint64_t OSPRayScene::_serializeSpheres(const size_t groupId,
                                        const size_t groupMaterialId,
                                        const size_t ospMaterialId)
{
    auto& group = _geometryGroups[groupId];
    auto& model = _ospGeometryGroups[groupId].model;
    auto& s = group.getSpheres();
    const auto& spheres = s[groupMaterialId];
    const auto bufferSize = spheres.size() * sizeof(Sphere);
    if (_ospExtendedSpheres.find(ospMaterialId) != _ospExtendedSpheres.end())
        ospRemoveGeometry(model, _ospExtendedSpheres[ospMaterialId]);

    _ospExtendedSpheres[ospMaterialId] = ospNewGeometry("extendedspheres");
    _ospExtendedSpheresData[ospMaterialId] =
        ospNewData(bufferSize / sizeof(float), OSP_FLOAT, spheres.data(),
                   _getOSPDataFlags());

    ospSetObject(_ospExtendedSpheres[ospMaterialId], "extendedspheres",
                 _ospExtendedSpheresData[ospMaterialId]);

    if (_ospMaterials[ospMaterialId])
        ospSetMaterial(_ospExtendedSpheres[ospMaterialId],
                       _ospMaterials[ospMaterialId]);

    ospCommit(_ospExtendedSpheres[ospMaterialId]);

    const auto& geometryParameters = _parametersManager.getGeometryParameters();
    if (geometryParameters.getCircuitUseSimulationModel())
        ospAddGeometry(_simulationModel, _ospExtendedSpheres[ospMaterialId]);
    else
        ospAddGeometry(model, _ospExtendedSpheres[ospMaterialId]);

    group.setSpheresDirty(false);
    return bufferSize;
}

uint64_t OSPRayScene::_serializeCylinders(const size_t groupId,
                                          const size_t groupMaterialId,
                                          const size_t ospMaterialId)
{
    auto& group = _geometryGroups[groupId];
    auto& model = _ospGeometryGroups[groupId].model;
    auto& c = group.getCylinders();
    const auto& cylinders = c[groupMaterialId];
    const auto bufferSize = cylinders.size() * sizeof(Cylinder);
    if (_ospExtendedCylinders.find(ospMaterialId) !=
        _ospExtendedCylinders.end())
        ospRemoveGeometry(model, _ospExtendedCylinders[ospMaterialId]);

    _ospExtendedCylinders[ospMaterialId] = ospNewGeometry("extendedcylinders");
    _ospExtendedCylindersData[ospMaterialId] =
        ospNewData(bufferSize / sizeof(float), OSP_FLOAT, cylinders.data(),
                   _getOSPDataFlags());
    ospSetObject(_ospExtendedCylinders[ospMaterialId], "extendedcylinders",
                 _ospExtendedCylindersData[ospMaterialId]);

    if (_ospMaterials[ospMaterialId])
        ospSetMaterial(_ospExtendedCylinders[ospMaterialId],
                       _ospMaterials[ospMaterialId]);

    ospCommit(_ospExtendedCylinders[ospMaterialId]);

    const auto& geometryParameters = _parametersManager.getGeometryParameters();
    if (geometryParameters.getCircuitUseSimulationModel())
        ospAddGeometry(_simulationModel, _ospExtendedCylinders[ospMaterialId]);
    else
        ospAddGeometry(model, _ospExtendedCylinders[ospMaterialId]);
    group.setCylindersDirty(false);
    return bufferSize;
}

uint64_t OSPRayScene::_serializeCones(const size_t groupId,
                                      const size_t groupMaterialId,
                                      const size_t ospMaterialId)
{
    auto& group = _geometryGroups[groupId];
    auto& model = _ospGeometryGroups[groupId].model;
    auto& c = group.getCones();
    const auto& cones = c[groupMaterialId];
    const auto bufferSize = cones.size() * sizeof(Cone);
    if (_ospExtendedCones.find(ospMaterialId) != _ospExtendedCones.end())
        ospRemoveGeometry(model, _ospExtendedCones[ospMaterialId]);

    _ospExtendedCones[ospMaterialId] = ospNewGeometry("extendedcones");
    _ospExtendedConesData[ospMaterialId] =
        ospNewData(bufferSize / sizeof(float), OSP_FLOAT, cones.data(),
                   _getOSPDataFlags());
    ospSetObject(_ospExtendedCones[ospMaterialId], "extendedcones",
                 _ospExtendedConesData[ospMaterialId]);

    if (_ospMaterials[ospMaterialId])
        ospSetMaterial(_ospExtendedCones[ospMaterialId],
                       _ospMaterials[ospMaterialId]);

    ospCommit(_ospExtendedCones[ospMaterialId]);

    const auto& geometryParameters = _parametersManager.getGeometryParameters();
    if (geometryParameters.getCircuitUseSimulationModel())
        ospAddGeometry(_simulationModel, _ospExtendedCones[ospMaterialId]);
    else
        ospAddGeometry(model, _ospExtendedCones[ospMaterialId]);
    group.setConesDirty(false);
    return bufferSize;
}

uint64_t OSPRayScene::_serializeMeshes(const size_t groupId,
                                       const size_t groupMaterialId,
                                       const size_t ospMaterialId)
{
    auto& group = _geometryGroups[groupId];
    auto& t = group.getTrianglesMeshes();
    uint64_t size = 0;
    _ospMeshes[ospMaterialId] = ospNewGeometry("trianglemesh");
    assert(_ospMeshes[ospMaterialId]);

    auto& trianglesMesh = t[groupMaterialId];
    size += trianglesMesh.vertices.size() * 3 * sizeof(float);
    OSPData vertices =
        ospNewData(trianglesMesh.vertices.size(), OSP_FLOAT3,
                   trianglesMesh.vertices.data(), _getOSPDataFlags());

    if (!trianglesMesh.normals.empty())
    {
        size += trianglesMesh.normals.size() * 3 * sizeof(float);
        OSPData normals =
            ospNewData(trianglesMesh.normals.size(), OSP_FLOAT3,
                       trianglesMesh.normals.data(), _getOSPDataFlags());
        ospSetObject(_ospMeshes[ospMaterialId], "vertex.normal", normals);
    }

    size += trianglesMesh.indices.size() * 3 * sizeof(int);
    OSPData indices =
        ospNewData(trianglesMesh.indices.size(), OSP_INT3,
                   trianglesMesh.indices.data(), _getOSPDataFlags());

    if (!trianglesMesh.colors.empty())
    {
        size += trianglesMesh.colors.size() * 4 * sizeof(float);
        OSPData colors =
            ospNewData(trianglesMesh.colors.size(), OSP_FLOAT3A,
                       trianglesMesh.colors.data(), _getOSPDataFlags());
        ospSetObject(_ospMeshes[ospMaterialId], "vertex.color", colors);
        ospRelease(colors);
    }

    if (!trianglesMesh.textureCoordinates.empty())
    {
        size += trianglesMesh.textureCoordinates.size() * 2 * sizeof(float);
        OSPData texCoords =
            ospNewData(trianglesMesh.textureCoordinates.size(), OSP_FLOAT2,
                       trianglesMesh.textureCoordinates.data(),
                       _getOSPDataFlags());
        ospSetObject(_ospMeshes[ospMaterialId], "vertex.texcoord", texCoords);
        ospRelease(texCoords);
    }

    ospSetObject(_ospMeshes[ospMaterialId], "position", vertices);
    ospRelease(vertices);
    ospSetObject(_ospMeshes[ospMaterialId], "index", indices);
    ospRelease(indices);
    ospSet1i(_ospMeshes[ospMaterialId], "alpha_type", 0);
    ospSet1i(_ospMeshes[ospMaterialId], "alpha_component", 4);

    if (_ospMaterials[ospMaterialId])
        ospSetMaterial(_ospMeshes[ospMaterialId], _ospMaterials[ospMaterialId]);

    ospCommit(_ospMeshes[ospMaterialId]);

    auto& model = _ospGeometryGroups[groupId].model;
    ospAddGeometry(model, _ospMeshes[ospMaterialId]);
    group.setTrianglesMeshesDirty(false);
    return size;
}

OSPModel OSPRayScene::_getActiveModel()
{
    auto model = _rootModel;
    const auto& geometryParameters = _parametersManager.getGeometryParameters();
    if (geometryParameters.getCircuitUseSimulationModel())
        model = _simulationModel;
    return model;
}

void OSPRayScene::_syncOSPModelsWithGeometryGroups()
{
    // Create new OSPModels
    for (auto& group : _geometryGroups)
    {
        bool found = false;
        for (auto& ospGroup : _ospGeometryGroups)
            if (ospGroup.geometryGroup == &group)
            {
                found = true;
                break;
            }

        if (!found)
        {
            GeometryGroupAttributes attr;
            attr.geometryGroup = &group;
            attr.model = ospNewModel();
            _ospGeometryGroups.push_back(attr);
        }
    }

    // Remove unused OSPModels
    auto it = _ospGeometryGroups.begin();
    while (it != _ospGeometryGroups.end())
    {
        bool found = false;
        for (const auto& group : _geometryGroups)
            if (&group == (*it).geometryGroup)
            {
                found = true;
                break;
            }
        if (found)
            ++it;
        else
            _ospGeometryGroups.erase(it);
    }
}

void OSPRayScene::serializeGeometry()
{
    _syncOSPModelsWithGeometryGroups();
    _sizeInBytes = 0;
    uint64_t materialId = _materialManager.getMaterials().size();
    for (size_t g = 0; g < _geometryGroups.size(); ++g)
    {
        auto& group = _geometryGroups[g];
        if (!group.enabled())
            continue;

        const auto nbMaterials =
            group.getMaterialManager().getMaterials().size();

        if (group.spheresDirty())
            for (size_t i = 0; i < nbMaterials; ++i)
                _sizeInBytes += _serializeSpheres(g, i, i + materialId);

        if (group.cylindersDirty())
            for (size_t i = 0; i < nbMaterials; ++i)
                _sizeInBytes += _serializeCylinders(g, i, i + materialId);

        if (group.conesDirty())
            for (size_t i = 0; i < nbMaterials; ++i)
                _sizeInBytes += _serializeCones(g, i, i + materialId);

        if (group.trianglesMeshesDirty())
            for (size_t i = 0; i < nbMaterials; ++i)
                _sizeInBytes += _serializeMeshes(g, i, i + materialId);
        materialId += nbMaterials;
    }
}

void OSPRayScene::buildGeometry()
{
    BRAYNS_INFO << "Building OSPRay geometry" << std::endl;

    commitMaterials();

    const auto& geomParams = _parametersManager.getGeometryParameters();

    _rootModel = ospNewModel();

    if (geomParams.getCircuitUseSimulationModel() && !_simulationModel)
        _simulationModel = ospNewModel();

    // Optix needs a bounding box around the volume so that if can find
    // intersections before initiating the traversal
    _processVolumeAABBGeometry();

    serializeGeometry();

    uint64_t totalNbSpheres = 0;
    uint64_t totalNbCylinders = 0;
    uint64_t totalNbCones = 0;
    uint64_t totalNbVertices = 0;
    uint64_t totalNbIndices = 0;
    for (auto& group : _geometryGroups)
    {
        for (const auto& spheres : group.getSpheres())
            totalNbSpheres += spheres.second.size();
        for (const auto& cylinders : group.getCylinders())
            totalNbCylinders += cylinders.second.size();
        for (const auto& cones : group.getCones())
            totalNbCones += cones.second.size();
        for (const auto& trianglesMeshes : group.getTrianglesMeshes())
        {
            totalNbVertices += trianglesMeshes.second.vertices.size();
            totalNbIndices += trianglesMeshes.second.indices.size();
        }
    }

    BRAYNS_INFO << "---------------------------------------------------"
                << std::endl;
    BRAYNS_INFO << "Geometry information" << std::endl;
    BRAYNS_INFO << "Spheres  : " << totalNbSpheres << std::endl;
    BRAYNS_INFO << "Cylinders: " << totalNbCylinders << std::endl;
    BRAYNS_INFO << "Cones    : " << totalNbCones << std::endl;
    BRAYNS_INFO << "Vertices : " << totalNbVertices << std::endl;
    BRAYNS_INFO << "Indices  : " << totalNbIndices << std::endl;
    BRAYNS_INFO << "Materials: " << _getNbMaterials() << std::endl;
    BRAYNS_INFO << "Total    : " << _sizeInBytes << " bytes ("
                << _sizeInBytes / 1048576 << " MB)" << std::endl;
    BRAYNS_INFO << "---------------------------------------------------"
                << std::endl;
}

void OSPRayScene::commitLights()
{
    size_t lightCount = 0;
    for (const auto& light : _lights)
    {
        DirectionalLight* directionalLight =
            dynamic_cast<DirectionalLight*>(light.get());
        if (directionalLight)
        {
            if (_ospLights.size() <= lightCount)
                _ospLights.push_back(ospNewLight(nullptr, "DirectionalLight"));

            const Vector3f color = directionalLight->getColor();
            ospSet3f(_ospLights[lightCount], "color", color.x(), color.y(),
                     color.z());
            const Vector3f direction = directionalLight->getDirection();
            ospSet3f(_ospLights[lightCount], "direction", direction.x(),
                     direction.y(), direction.z());
            ospSet1f(_ospLights[lightCount], "intensity",
                     directionalLight->getIntensity());
            ospCommit(_ospLights[lightCount]);
            ++lightCount;
        }
        else
        {
            PointLight* pointLight = dynamic_cast<PointLight*>(light.get());
            if (pointLight)
            {
                if (_ospLights.size() <= lightCount)
                    _ospLights.push_back(ospNewLight(nullptr, "PointLight"));

                const Vector3f position = pointLight->getPosition();
                ospSet3f(_ospLights[lightCount], "position", position.x(),
                         position.y(), position.z());
                const Vector3f color = pointLight->getColor();
                ospSet3f(_ospLights[lightCount], "color", color.x(), color.y(),
                         color.z());
                ospSet1f(_ospLights[lightCount], "intensity",
                         pointLight->getIntensity());
                ospSet1f(_ospLights[lightCount], "radius",
                         pointLight->getCutoffDistance());
                ospCommit(_ospLights[lightCount]);
                ++lightCount;
            }
        }
    }

    if (!_ospLightData)
    {
        _ospLightData = ospNewData(_ospLights.size(), OSP_OBJECT,
                                   &_ospLights[0], _getOSPDataFlags());
        ospCommit(_ospLightData);
        for (auto renderer : _renderers)
        {
            auto impl =
                std::static_pointer_cast<OSPRayRenderer>(renderer)->impl();
            ospSetData(impl, "lights", _ospLightData);
        }
    }
}

void OSPRayScene::_commitOSPMaterial(OSPMaterial ospMaterial,
                                     Material& material)
{
    Vector3f value3f = material.getColor();
    ospSet3f(ospMaterial, "kd", value3f.x(), value3f.y(), value3f.z());
    value3f = material.getSpecularColor();
    ospSet3f(ospMaterial, "ks", value3f.x(), value3f.y(), value3f.z());
    ospSet1f(ospMaterial, "ns", material.getSpecularExponent());
    ospSet1f(ospMaterial, "d", material.getOpacity());
    ospSet1f(ospMaterial, "refraction", material.getRefractionIndex());
    ospSet1f(ospMaterial, "reflection", material.getReflectionIndex());
    ospSet1f(ospMaterial, "a", material.getEmission());
    ospSet1f(ospMaterial, "glossiness", material.getGlossiness());
    ospSet1i(ospMaterial, "cast_simulation_data",
             material.getCastSimulationData());
    ospSet1i(ospMaterial, "skybox", material.getType() == MaterialType::skybox);

    for (const auto& textureType : textureTypeMaterialAttribute)
        ospSetObject(ospMaterial, textureType.attribute.c_str(), nullptr);

    // Textures
    for (auto texture : material.getTextures())
    {
        if (texture.second != TEXTURE_NAME_SIMULATION)
            ImageManager::importTextureFromFile(_textures, texture.first,
                                                texture.second);
        else
            BRAYNS_ERROR << "Failed to load texture: " << texture.second
                         << std::endl;

        OSPTexture2D ospTexture = _createTexture2D(texture.second);
        ospSetObject(
            ospMaterial,
            textureTypeMaterialAttribute[texture.first].attribute.c_str(),
            ospTexture);

        BRAYNS_DEBUG << "Texture assigned to "
                     << textureTypeMaterialAttribute[texture.first].attribute
                     << ": " << texture.second << std::endl;
    }
    ospCommit(ospMaterial);
}

void OSPRayScene::commitMaterials(const Action action)
{
    auto nbMaterials = _getNbMaterials();

    if (action == Action::create)
    {
        // Create materials
        for (auto& material : _ospMaterials)
            ospRelease(material);
        _ospMaterials.clear();
        _ospMaterials.reserve(nbMaterials);
        for (size_t i = 0; i < nbMaterials; ++i)
        {
            auto ospMaterial = ospNewMaterial(nullptr, "ExtendedOBJMaterial");
            _ospMaterials.push_back(ospMaterial);
        }
    }

    size_t materialId = 0;
    // Scene materials
    for (auto& material : _materialManager.getMaterials())
    {
        auto& ospMaterial = _ospMaterials[materialId];
        _commitOSPMaterial(ospMaterial, material);
        ++materialId;
    }

    // Geometry materials
    for (auto& group : _geometryGroups)
        for (auto& material : group.getMaterialManager().getMaterials())
        {
            auto& ospMaterial = _ospMaterials[materialId];
            _commitOSPMaterial(ospMaterial, material);
            ++materialId;
        }

    _ospMaterialData = ospNewData(nbMaterials, OSP_OBJECT, &_ospMaterials[0],
                                  _getOSPDataFlags());
    ospCommit(_ospMaterialData);

    for (const auto& renderer : _renderers)
    {
        auto impl = std::static_pointer_cast<OSPRayRenderer>(renderer)->impl();
        ospSetData(impl, "materials", _ospMaterialData);
        ospCommit(impl);
    }
    markModified();
}

void OSPRayScene::commitTransferFunctionData()
{
    if (_ospTransferFunctionDiffuseData)
        ospRelease(_ospTransferFunctionDiffuseData);

    if (_ospTransferFunctionEmissionData)
        ospRelease(_ospTransferFunctionEmissionData);

    _ospTransferFunctionDiffuseData =
        ospNewData(_transferFunction.getDiffuseColors().size(), OSP_FLOAT4,
                   _transferFunction.getDiffuseColors().data(),
                   _getOSPDataFlags());
    ospCommit(_ospTransferFunctionDiffuseData);

    _ospTransferFunctionEmissionData =
        ospNewData(_transferFunction.getEmissionIntensities().size(),
                   OSP_FLOAT3,
                   _transferFunction.getEmissionIntensities().data(),
                   _getOSPDataFlags());
    ospCommit(_ospTransferFunctionEmissionData);

    for (const auto& renderer : _renderers)
    {
        auto impl = std::static_pointer_cast<OSPRayRenderer>(renderer)->impl();

        // Transfer function Diffuse colors
        ospSetData(impl, "transferFunctionDiffuseData",
                   _ospTransferFunctionDiffuseData);

        // Transfer function emission data
        ospSetData(impl, "transferFunctionEmissionData",
                   _ospTransferFunctionEmissionData);

        // Transfer function size
        ospSet1i(impl, "transferFunctionSize",
                 _transferFunction.getDiffuseColors().size());

        // Transfer function range
        ospSet1f(impl, "transferFunctionMinValue",
                 _transferFunction.getValuesRange().x());
        ospSet1f(impl, "transferFunctionRange",
                 _transferFunction.getValuesRange().y() -
                     _transferFunction.getValuesRange().x());
        ospCommit(impl);
    }
    markModified();
}

void OSPRayScene::commitVolumeData()
{
    VolumeHandlerPtr volumeHandler = getVolumeHandler();
    if (!volumeHandler)
        return;

    const auto& vp = _parametersManager.getVolumeParameters();
    if (vp.isModified())
    {
        // Cleanup existing volume data in handler and renderers
        volumeHandler->clear();

        // An empty array has to be assigned to the renderers
        _ospVolumeDataSize = 0;
        _ospVolumeData =
            ospNewData(_ospVolumeDataSize, OSP_UCHAR, 0, _getOSPDataFlags());
        ospCommit(_ospVolumeData);
        for (const auto& renderer : _renderers)
        {
            auto impl =
                std::static_pointer_cast<OSPRayRenderer>(renderer)->impl();
            ospSetData(impl, "volumeData", _ospVolumeData);
        }
    }

    const auto& ap = _parametersManager.getAnimationParameters();
    const auto animationFrame = ap.getFrame();
    volumeHandler->setCurrentIndex(animationFrame);
    void* data = volumeHandler->getData();
    if (data && _ospVolumeDataSize == 0)
    {
        // Set volume data to renderers
        _ospVolumeDataSize = volumeHandler->getSize();
        _ospVolumeData =
            ospNewData(_ospVolumeDataSize, OSP_UCHAR, data, _getOSPDataFlags());
        ospCommit(_ospVolumeData);
        for (const auto& renderer : _renderers)
        {
            auto impl =
                std::static_pointer_cast<OSPRayRenderer>(renderer)->impl();

            ospSetData(impl, "volumeData", _ospVolumeData);
            const auto& dimensions = volumeHandler->getDimensions();
            ospSet3i(impl, "volumeDimensions", dimensions.x(), dimensions.y(),
                     dimensions.z());
            const auto& elementSpacing =
                _parametersManager.getVolumeParameters().getElementSpacing();
            ospSet3f(impl, "volumeElementSpacing", elementSpacing.x(),
                     elementSpacing.y(), elementSpacing.z());
            const auto& offset =
                _parametersManager.getVolumeParameters().getOffset();
            ospSet3f(impl, "volumeOffset", offset.x(), offset.y(), offset.z());
            const auto epsilon = volumeHandler->getEpsilon(
                elementSpacing,
                _parametersManager.getRenderingParameters().getSamplesPerRay());
            ospSet1f(impl, "volumeEpsilon", epsilon);
        }
        markModified();
    }
}

void OSPRayScene::commitSimulationData()
{
    if (!_simulationHandler)
        return;

    const auto animationFrame =
        _parametersManager.getAnimationParameters().getFrame();

    if (_simulationHandler->getCurrentFrame() == animationFrame)
        return;

    auto frameData = _simulationHandler->getFrameData(animationFrame);

    if (!frameData)
        return;

    if (_ospSimulationData)
        ospRelease(_ospSimulationData);
    _ospSimulationData = ospNewData(_simulationHandler->getFrameSize(),
                                    OSP_FLOAT, frameData, _getOSPDataFlags());
    ospCommit(_ospSimulationData);

    for (const auto& renderer : _renderers)
    {
        auto impl = std::static_pointer_cast<OSPRayRenderer>(renderer)->impl();
        ospSetData(impl, "simulationData", _ospSimulationData);
        ospSet1i(impl, "simulationDataSize",
                 _simulationHandler->getFrameSize());
    }
    markModified();
}

OSPTexture2D OSPRayScene::_createTexture2D(const std::string& textureName)
{
    if (_ospTextures.find(textureName) != _ospTextures.end())
        return _ospTextures[textureName];

    Texture2DPtr texture = _textures[textureName];
    if (!texture)
    {
        BRAYNS_WARN << "Texture " << textureName << " is not in the cache"
                    << std::endl;
        return nullptr;
    }

    OSPTextureFormat type = OSP_TEXTURE_R8; // smallest valid type as default
    if (texture->getDepth() == 1)
    {
        if (texture->getNbChannels() == 3)
            type = OSP_TEXTURE_SRGB;
        if (texture->getNbChannels() == 4)
            type = OSP_TEXTURE_SRGBA;
    }
    else if (texture->getDepth() == 4)
    {
        if (texture->getNbChannels() == 3)
            type = OSP_TEXTURE_RGB32F;
        if (texture->getNbChannels() == 4)
            type = OSP_TEXTURE_RGBA32F;
    }

    BRAYNS_DEBUG << "Creating OSPRay texture from " << textureName << " :"
                 << texture->getWidth() << "x" << texture->getHeight() << "x"
                 << (int)type << std::endl;

    osp::vec2i texSize{int(texture->getWidth()), int(texture->getHeight())};
    OSPTexture2D ospTexture =
        ospNewTexture2D(texSize, type, texture->getRawData(), 0);

    assert(ospTexture);
    ospCommit(ospTexture);

    _ospTextures[textureName] = ospTexture;

    return ospTexture;
}

bool OSPRayScene::isVolumeSupported(const std::string& volumeFile) const
{
    return boost::algorithm::ends_with(volumeFile, ".raw");
}

uint32_t OSPRayScene::_getOSPDataFlags()
{
    return _parametersManager.getGeometryParameters().getMemoryMode() ==
                   MemoryMode::shared
               ? OSP_DATA_SHARED_BUFFER
               : 0;
}
}
