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
#include "OSPRayGeometryGroup.h"
#include "OSPRayRenderer.h"

#include <brayns/common/geometry/GeometryGroup.h>
#include <brayns/common/light/DirectionalLight.h>
#include <brayns/common/light/PointLight.h>
#include <brayns/common/log.h>
#include <brayns/common/simulation/AbstractSimulationHandler.h>
#include <brayns/common/volume/VolumeHandler.h>
#include <brayns/io/ImageManager.h>
#include <brayns/parameters/GeometryParameters.h>
#include <brayns/parameters/SceneParameters.h>

#include <boost/algorithm/string/predicate.hpp> // ends_with

namespace brayns
{
OSPRayScene::OSPRayScene(Renderers renderers,
                         ParametersManager& parametersManager)
    : Scene(renderers, parametersManager)
{
    _materialManager =
        std::make_shared<OSPRayMaterialManager>(_getOSPDataFlags());
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
    BRAYNS_FCT_ENTRY

    Scene::unload();

    if (_ospSimulationData)
        ospRelease(_ospSimulationData);

    if (_ospVolumeData)
        ospRelease(_ospVolumeData);
}

OSPModel OSPRayScene::_getActiveModel()
{
    BRAYNS_FCT_ENTRY

    auto model = _rootModel;
    const auto& geometryParameters = _parametersManager.getGeometryParameters();
    if (geometryParameters.getCircuitUseSimulationModel())
        model = _rootSimulationModel;
    return model;
}

void OSPRayScene::buildGeometry()
{
    BRAYNS_FCT_ENTRY

    commitMaterials();

    // Optix needs a bounding box around the volume so that if can find
    // intersections before initiating the traversal
    _processVolumeAABBGeometry();

    for (size_t i = 0; i < _geometryGroupAttributes.size(); ++i)
    {
        const auto& groupAttributes = _geometryGroupAttributes[i];
        if (groupAttributes.enabled())
            _geometryGroups[i]->logInformation();
    }
}

void OSPRayScene::commit()
{
    BRAYNS_FCT_ENTRY

    if (_rootModel)
        ospRelease(_rootModel);
    _rootModel = ospNewModel();

    if (_rootSimulationModel)
        ospRelease(_rootSimulationModel);
    _rootSimulationModel = nullptr;

    for (size_t g = 0; g < _geometryGroupAttributes.size(); ++g)
    {
        auto& groupAttributes = _geometryGroupAttributes[g];
        if (groupAttributes.enabled())
        {
            auto impl = std::static_pointer_cast<OSPRayGeometryGroup>(
                _geometryGroups[g]);
            impl->commit();

            for (size_t i = 0; i < groupAttributes.transformations().size();
                 ++i)
            {
                auto& transform = groupAttributes.transformations()[i];
                if (groupAttributes.boundingBox())
                    ospAddGeometry(_rootModel,
                                   impl->getBoundingBoxModelInstance(
                                       transform.translation(),
                                       transform.rotation(),
                                       transform.scale()));
                if (groupAttributes.visible())
                    ospAddGeometry(_rootModel,
                                   impl->getInstance(i, transform.translation(),
                                                     transform.rotation(),
                                                     transform.scale()));
            }

            if (impl->useSimulationModel())
            {
                auto& transform = groupAttributes.transformations()[0];
                if (!_rootSimulationModel)
                    _rootSimulationModel = ospNewModel();
                ospAddGeometry(_rootSimulationModel,
                               impl->getSimulationModelInstance(
                                   transform.translation(),
                                   transform.rotation(), transform.scale()));
            }
        }
    }

    BRAYNS_DEBUG << "Committing root models" << std::endl;
    ospCommit(_rootModel);
    if (_rootSimulationModel)
        ospCommit(_rootSimulationModel);
}

void OSPRayScene::commitLights()
{
    BRAYNS_FCT_ENTRY

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

void OSPRayScene::commitMaterials()
{
    if (!_materialManager->isModified())
        return;

    BRAYNS_FCT_ENTRY

    auto impl =
        std::static_pointer_cast<OSPRayMaterialManager>(_materialManager);
    impl->commit();
    auto materialData = impl->getData();

    for (const auto& renderer : _renderers)
    {
        auto rendererImpl =
            std::static_pointer_cast<OSPRayRenderer>(renderer)->impl();
        ospSetData(rendererImpl, "materials", materialData);
        ospCommit(rendererImpl);
    }
    _materialManager->resetModified();
}

void OSPRayScene::commitTransferFunctionData()
{
    BRAYNS_FCT_ENTRY

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
    const auto volumeHandler = getVolumeHandler();
    if (!volumeHandler)
        return;

    BRAYNS_FCT_ENTRY

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
    auto data = volumeHandler->getData();
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

    BRAYNS_FCT_ENTRY

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

bool OSPRayScene::isVolumeSupported(const std::string& volumeFile) const
{
    BRAYNS_FCT_ENTRY

    return boost::algorithm::ends_with(volumeFile, ".raw");
}

uint32_t OSPRayScene::_getOSPDataFlags()
{
    BRAYNS_FCT_ENTRY

    return _parametersManager.getGeometryParameters().getMemoryMode() ==
                   MemoryMode::shared
               ? OSP_DATA_SHARED_BUFFER
               : 0;
}

GeometryGroupPtr OSPRayScene::addGeometryGroup(const std::string& name,
                                               const std::string& uri)
{
    BRAYNS_FCT_ENTRY

    GroupAttributes groupAttributes(name, uri, true);
#if 0
    srand(time(0));
    for (size_t i = 0; i < 99999; ++i)
    {
        const size_t s = 1500;
        const float h = s / 2.f;
        GroupTransformation transform;
        transform.translation(Vector3f(float(rand() % s) - h,
                                       float(rand() % s) - h,
                                       float(rand() % s) - h));
        transform.rotation(
            Vector3f(rand() % s - h, rand() % s - h, rand() % s - h));
        //        const size_t scale = (rand() % 30 - 15) / 2.f;
        //        transform.scale(Vector3f(scale, scale, scale));
        groupAttributes.transformations().push_back(transform);
    }
#endif
    _geometryGroupAttributes.push_back(groupAttributes);
    _geometryGroups.push_back(
        std::make_shared<OSPRayGeometryGroup>(_materialManager));
    return _geometryGroups[_geometryGroups.size() - 1];
}

void OSPRayScene::removeGeometryGroup(const size_t index)
{
    BRAYNS_FCT_ENTRY

    _geometryGroupAttributes.erase(_geometryGroupAttributes.begin() + index);
    _geometryGroups.erase(_geometryGroups.begin() + index);
}
}
