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

#include <brayns/common/light/DirectionalLight.h>
#include <brayns/common/light/PointLight.h>
#include <brayns/common/log.h>
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
    , _model(nullptr)
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
    if (_model)
    {
        for (size_t materialId = 0; materialId < _materials.size();
             ++materialId)
        {
            if (_ospMeshes[materialId])
                ospRemoveGeometry(_model, _ospMeshes[materialId]);
            if (_ospExtendedSpheres[materialId])
                ospRemoveGeometry(_model, _ospExtendedSpheres[materialId]);
            if (_ospExtendedCylinders[materialId])
                ospRemoveGeometry(_model, _ospExtendedCylinders[materialId]);
            if (_ospExtendedCones[materialId])
                ospRemoveGeometry(_model, _ospExtendedCones[materialId]);
        }
        ospCommit(_model);
        ospRelease(_model);
        _model = nullptr;
    }

    if (_simulationModel)
    {
        for (size_t materialId = 0; materialId < _materials.size();
             ++materialId)
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
    if (_model)
    {
        BRAYNS_INFO << "Committing main model" << std::endl;
        ospCommit(_model);
    }

    if (_simulationModel)
    {
        BRAYNS_INFO << "Committing simulation model" << std::endl;
        ospCommit(_simulationModel);
    }
}

uint64_t OSPRayScene::_serializeSpheres(const size_t materialId)
{
    if (_spheres.find(materialId) == _spheres.end())
        return 0;

    auto model = _getActiveModel();
    const auto& spheres = _spheres[materialId];
    const auto bufferSize = spheres.size() * sizeof(Sphere);
    if (_ospExtendedSpheres.find(materialId) != _ospExtendedSpheres.end())
        ospRemoveGeometry(model, _ospExtendedSpheres[materialId]);

    _ospExtendedSpheres[materialId] = ospNewGeometry("extendedspheres");
    _ospExtendedSpheresData[materialId] =
        ospNewData(bufferSize / sizeof(float), OSP_FLOAT, spheres.data(),
                   _getOSPDataFlags());

    ospSetObject(_ospExtendedSpheres[materialId], "extendedspheres",
                 _ospExtendedSpheresData[materialId]);

    if (_ospMaterials[materialId])
        ospSetMaterial(_ospExtendedSpheres[materialId],
                       _ospMaterials[materialId]);

    ospCommit(_ospExtendedSpheres[materialId]);

    const auto& geometryParameters = _parametersManager.getGeometryParameters();
    if (geometryParameters.getCircuitUseSimulationModel())
        ospAddGeometry(_simulationModel, _ospExtendedSpheres[materialId]);
    else
        ospAddGeometry(model, _ospExtendedSpheres[materialId]);

    return bufferSize;
}

uint64_t OSPRayScene::_serializeCylinders(const size_t materialId)
{
    if (_cylinders.find(materialId) == _cylinders.end())
        return 0;

    auto model = _getActiveModel();
    const auto& cylinders = _cylinders[materialId];
    const auto bufferSize = cylinders.size() * sizeof(Cylinder);
    if (_ospExtendedCylinders.find(materialId) != _ospExtendedCylinders.end())
        ospRemoveGeometry(model, _ospExtendedCylinders[materialId]);

    _ospExtendedCylinders[materialId] = ospNewGeometry("extendedcylinders");
    _ospExtendedCylindersData[materialId] =
        ospNewData(bufferSize / sizeof(float), OSP_FLOAT, cylinders.data(),
                   _getOSPDataFlags());
    ospSetObject(_ospExtendedCylinders[materialId], "extendedcylinders",
                 _ospExtendedCylindersData[materialId]);

    if (_ospMaterials[materialId])
        ospSetMaterial(_ospExtendedCylinders[materialId],
                       _ospMaterials[materialId]);

    ospCommit(_ospExtendedCylinders[materialId]);

    const auto& geometryParameters = _parametersManager.getGeometryParameters();
    if (geometryParameters.getCircuitUseSimulationModel())
        ospAddGeometry(_simulationModel, _ospExtendedCylinders[materialId]);
    else
        ospAddGeometry(model, _ospExtendedCylinders[materialId]);
    return bufferSize;
}

uint64_t OSPRayScene::_serializeCones(const size_t materialId)
{
    if (_cones.find(materialId) == _cones.end())
        return 0;

    auto model = _getActiveModel();
    const auto& cones = _cones[materialId];
    const auto bufferSize = cones.size() * sizeof(Cone);
    if (_ospExtendedCones.find(materialId) != _ospExtendedCones.end())
        ospRemoveGeometry(model, _ospExtendedCones[materialId]);

    _ospExtendedCones[materialId] = ospNewGeometry("extendedcones");
    _ospExtendedConesData[materialId] =
        ospNewData(bufferSize / sizeof(float), OSP_FLOAT, cones.data(),
                   _getOSPDataFlags());
    ospSetObject(_ospExtendedCones[materialId], "extendedcones",
                 _ospExtendedConesData[materialId]);

    if (_ospMaterials[materialId])
        ospSetMaterial(_ospExtendedCones[materialId],
                       _ospMaterials[materialId]);

    ospCommit(_ospExtendedCones[materialId]);

    const auto& geometryParameters = _parametersManager.getGeometryParameters();
    if (geometryParameters.getCircuitUseSimulationModel())
        ospAddGeometry(_simulationModel, _ospExtendedCones[materialId]);
    else
        ospAddGeometry(model, _ospExtendedCones[materialId]);
    return bufferSize;
}

uint64_t OSPRayScene::_serializeMeshes(const size_t materialId)
{
    if (_trianglesMeshes.find(materialId) == _trianglesMeshes.end())
        return 0;

    uint64_t size = 0;
    _ospMeshes[materialId] = ospNewGeometry("trianglemesh");
    assert(_ospMeshes[materialId]);

    auto& trianglesMesh = _trianglesMeshes[materialId];
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
        ospSetObject(_ospMeshes[materialId], "vertex.normal", normals);
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
        ospSetObject(_ospMeshes[materialId], "vertex.color", colors);
        ospRelease(colors);
    }

    if (!trianglesMesh.textureCoordinates.empty())
    {
        size += trianglesMesh.textureCoordinates.size() * 2 * sizeof(float);
        OSPData texCoords =
            ospNewData(trianglesMesh.textureCoordinates.size(), OSP_FLOAT2,
                       trianglesMesh.textureCoordinates.data(),
                       _getOSPDataFlags());
        ospSetObject(_ospMeshes[materialId], "vertex.texcoord", texCoords);
        ospRelease(texCoords);
    }

    ospSetObject(_ospMeshes[materialId], "position", vertices);
    ospRelease(vertices);
    ospSetObject(_ospMeshes[materialId], "index", indices);
    ospRelease(indices);
    ospSet1i(_ospMeshes[materialId], "alpha_type", 0);
    ospSet1i(_ospMeshes[materialId], "alpha_component", 4);

    if (_ospMaterials[materialId])
        ospSetMaterial(_ospMeshes[materialId], _ospMaterials[materialId]);

    ospCommit(_ospMeshes[materialId]);

    ospAddGeometry(_model, _ospMeshes[materialId]);
    return size;
}

OSPModel OSPRayScene::_getActiveModel()
{
    auto model = _model;
    const auto& geometryParameters = _parametersManager.getGeometryParameters();
    if (geometryParameters.getCircuitUseSimulationModel())
        model = _simulationModel;
    return model;
}

void OSPRayScene::serializeGeometry()
{
    _sizeInBytes = 0;
    if (_spheresDirty)
        for (size_t i = 0; i < _materials.size(); ++i)
            _sizeInBytes += _serializeSpheres(i);

    if (_cylindersDirty)
        for (size_t i = 0; i < _materials.size(); ++i)
            _sizeInBytes += _serializeCylinders(i);

    if (_conesDirty)
        for (size_t i = 0; i < _materials.size(); ++i)
            _sizeInBytes += _serializeCones(i);

    if (_trianglesMeshesDirty)
        for (size_t i = 0; i < _materials.size(); ++i)
            _sizeInBytes += _serializeMeshes(i);

    _spheresDirty = false;
    _cylindersDirty = false;
    _conesDirty = false;
    _trianglesMeshesDirty = false;
}

void OSPRayScene::buildGeometry()
{
    BRAYNS_INFO << "Building OSPRay geometry" << std::endl;

    commitMaterials();

    const auto& geomParams = _parametersManager.getGeometryParameters();

    _model = ospNewModel();

    if (geomParams.getCircuitUseSimulationModel() && !_simulationModel)
        _simulationModel = ospNewModel();

    // Optix needs a bounding box around the volume so that if can find
    // intersections before initiating the traversal
    _processVolumeAABBGeometry();

    serializeGeometry();

    size_t totalNbSpheres = 0;
    size_t totalNbCylinders = 0;
    size_t totalNbCones = 0;
    size_t totalNbVertices = 0;
    size_t totalNbIndices = 0;
    for (const auto& spheres : _spheres)
        totalNbSpheres += spheres.second.size();
    for (const auto& cylinders : _cylinders)
        totalNbCylinders += cylinders.second.size();
    for (const auto& cones : _cones)
        totalNbCones += cones.second.size();
    for (const auto& trianglesMeshes : _trianglesMeshes)
    {
        totalNbVertices += trianglesMeshes.second.vertices.size();
        totalNbIndices += trianglesMeshes.second.indices.size();
    }

    BRAYNS_INFO << "---------------------------------------------------"
                << std::endl;
    BRAYNS_INFO << "Geometry information" << std::endl;
    BRAYNS_INFO << "Spheres  : " << totalNbSpheres << std::endl;
    BRAYNS_INFO << "Cylinders: " << totalNbCylinders << std::endl;
    BRAYNS_INFO << "Cones    : " << totalNbCones << std::endl;
    BRAYNS_INFO << "Vertices : " << totalNbVertices << std::endl;
    BRAYNS_INFO << "Indices  : " << totalNbIndices << std::endl;
    BRAYNS_INFO << "Materials: " << _materials.size() << std::endl;
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
            OSPRayRenderer* osprayRenderer =
                dynamic_cast<OSPRayRenderer*>(renderer.get());
            ospSetData(osprayRenderer->impl(), "lights", _ospLightData);
        }
    }
}

void OSPRayScene::commitMaterials(const Action action)
{
    if (action == Action::create)
    {
        // Create materials
        for (auto& material : _ospMaterials)
            ospRelease(material);
        _ospMaterials.clear();
        _ospMaterials.reserve(_materials.size());
        for (size_t i = 0; i < _materials.size(); ++i)
        {
            auto ospMaterial = ospNewMaterial(nullptr, "ExtendedOBJMaterial");
            _ospMaterials.push_back(ospMaterial);
        }
    }

    for (size_t i = 0; i < _materials.size(); ++i)
    {
        // update material
        auto& ospMaterial = _ospMaterials[i];
        auto& material = _materials[i];

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
        ospSet1i(ospMaterial, "skybox",
                 material.getType() == MaterialType::skybox);

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

            BRAYNS_DEBUG
                << "Texture assigned to "
                << textureTypeMaterialAttribute[texture.first].attribute
                << " of material " << i << ": " << texture.second << std::endl;
        }
        ospCommit(ospMaterial);
    }

    _ospMaterialData = ospNewData(_materials.size(), OSP_OBJECT,
                                  &_ospMaterials[0], _getOSPDataFlags());
    ospCommit(_ospMaterialData);

    for (const auto& renderer : _renderers)
    {
        OSPRayRenderer* osprayRenderer =
            dynamic_cast<OSPRayRenderer*>(renderer.get());
        ospSetData(osprayRenderer->impl(), "materials", _ospMaterialData);
        ospCommit(osprayRenderer->impl());
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
        OSPRayRenderer* osprayRenderer =
            dynamic_cast<OSPRayRenderer*>(renderer.get());

        // Transfer function Diffuse colors
        ospSetData(osprayRenderer->impl(), "transferFunctionDiffuseData",
                   _ospTransferFunctionDiffuseData);

        // Transfer function emission data
        ospSetData(osprayRenderer->impl(), "transferFunctionEmissionData",
                   _ospTransferFunctionEmissionData);

        // Transfer function size
        ospSet1i(osprayRenderer->impl(), "transferFunctionSize",
                 _transferFunction.getDiffuseColors().size());

        // Transfer function range
        ospSet1f(osprayRenderer->impl(), "transferFunctionMinValue",
                 _transferFunction.getValuesRange().x());
        ospSet1f(osprayRenderer->impl(), "transferFunctionRange",
                 _transferFunction.getValuesRange().y() -
                     _transferFunction.getValuesRange().x());

        ospCommit(osprayRenderer->impl());
    }
    markModified();
}

void OSPRayScene::commitVolumeData()
{
    VolumeHandlerPtr volumeHandler = getVolumeHandler();
    if (!volumeHandler)
        return;

    const auto& ap = _parametersManager.getAnimationParameters();
    const auto& vp = _parametersManager.getVolumeParameters();
    const auto animationFrame = ap.getFrame();
    volumeHandler->setCurrentIndex(animationFrame);
    void* data = volumeHandler->getData();
    if (data && (ap.isModified() || vp.isModified()))
    {
        for (const auto& renderer : _renderers)
        {
            OSPRayRenderer* osprayRenderer =
                dynamic_cast<OSPRayRenderer*>(renderer.get());

            const size_t size = volumeHandler->getSize();

            _ospVolumeData =
                ospNewData(size, OSP_UCHAR, data, _getOSPDataFlags());
            ospCommit(_ospVolumeData);
            ospSetData(osprayRenderer->impl(), "volumeData", _ospVolumeData);

            const Vector3ui& dimensions = volumeHandler->getDimensions();
            ospSet3i(osprayRenderer->impl(), "volumeDimensions", dimensions.x(),
                     dimensions.y(), dimensions.z());

            const Vector3f& elementSpacing =
                _parametersManager.getVolumeParameters().getElementSpacing();
            ospSet3f(osprayRenderer->impl(), "volumeElementSpacing",
                     elementSpacing.x(), elementSpacing.y(),
                     elementSpacing.z());

            const Vector3f& offset =
                _parametersManager.getVolumeParameters().getOffset();
            ospSet3f(osprayRenderer->impl(), "volumeOffset", offset.x(),
                     offset.y(), offset.z());

            const float epsilon = volumeHandler->getEpsilon(
                elementSpacing,
                _parametersManager.getVolumeParameters().getSamplesPerRay());
            ospSet1f(osprayRenderer->impl(), "volumeEpsilon", epsilon);
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
        OSPRayRenderer* osprayRenderer =
            dynamic_cast<OSPRayRenderer*>(renderer.get());

        ospSetData(osprayRenderer->impl(), "simulationData",
                   _ospSimulationData);
        ospSet1i(osprayRenderer->impl(), "simulationDataSize",
                 _simulationHandler->getFrameSize());
        ospCommit(osprayRenderer->impl());
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
