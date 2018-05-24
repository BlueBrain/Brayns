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
#include "OSPRayMaterial.h"
#include "OSPRayModel.h"
#include "OSPRayRenderer.h"
#include "utils.h"

#include <brayns/common/ImageManager.h>
#include <brayns/common/Transformation.h>
#include <brayns/common/light/DirectionalLight.h>
#include <brayns/common/light/PointLight.h>
#include <brayns/common/log.h>
#include <brayns/common/scene/Model.h>
#include <brayns/common/simulation/AbstractSimulationHandler.h>
#include <brayns/common/volume/VolumeHandler.h>

#include <brayns/parameters/GeometryParameters.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/parameters/SceneParameters.h>

#include <boost/algorithm/string/predicate.hpp> // ends_with

namespace brayns
{
OSPRayScene::OSPRayScene(const Renderers& renderers,
                         ParametersManager& parametersManager,
                         const size_t memoryManagementFlags)
    : Scene(renderers, parametersManager)
    , _memoryManagementFlags(memoryManagementFlags)
{
    _backgroundMaterial = std::make_shared<OSPRayMaterial>();
}

OSPRayScene::~OSPRayScene()
{
    if (_ospSimulationData)
        ospRelease(_ospSimulationData);

    if (_ospVolumeData)
        ospRelease(_ospVolumeData);

    if (_ospTransferFunctionDiffuseData)
        ospRelease(_ospTransferFunctionDiffuseData);

    if (_ospTransferFunctionEmissionData)
        ospRelease(_ospTransferFunctionEmissionData);

    for (auto& light : _ospLights)
        ospRelease(light);
    _ospLights.clear();

    if (_rootModel)
        ospRelease(_rootModel);

    if (_rootSimulationModel)
        ospRelease(_rootSimulationModel);
}

void OSPRayScene::commit()
{
    const bool isModified_ = isModified();

    commitVolumeData();
    commitSimulationData();
    commitTransferFunctionData();

    if (!isModified_)
        return;

    if (_rootModel)
        ospRelease(_rootModel);
    _rootModel = ospNewModel();

    if (_rootSimulationModel)
        ospRelease(_rootSimulationModel);
    _rootSimulationModel = nullptr;

    std::shared_lock<std::shared_timed_mutex> lock(_modelMutex);
    for (auto modelDescriptor : _modelDescriptors)
    {
        if (!modelDescriptor->getEnabled())
            continue;

        auto& impl = static_cast<OSPRayModel&>(modelDescriptor->getModel());
        const auto& transformation = modelDescriptor->getTransformation();

        BRAYNS_INFO << "Committing " << modelDescriptor->getName() << std::endl;

        if (modelDescriptor->getVisible() && impl.getUseSimulationModel())
        {
            if (!_rootSimulationModel)
                _rootSimulationModel = ospNewModel();
            addInstance(_rootSimulationModel, impl.getSimulationModel(),
                        transformation);
        }

        Boxf instancesBounds;
        const auto& modelBounds = modelDescriptor->getModel().getBounds();
        for (const auto& instance : modelDescriptor->getInstances())
        {
            const auto instanceTransform =
                transformation * instance.getTransformation();

            if (modelDescriptor->getBoundingBox() && instance.getBoundingBox())
                addInstance(_rootModel, impl.getBoundingBoxModel(),
                            instanceTransform);

            if (modelDescriptor->getVisible() && instance.getVisible())
                addInstance(_rootModel, impl.getModel(), instanceTransform);

            instancesBounds.merge(transformBox(modelBounds, instanceTransform));
        }

        if (modelDescriptor->getBoundingBox())
        {
            Transformation instancesTransform;
            instancesTransform.setTranslation(instancesBounds.getCenter() -
                                              modelBounds.getCenter());
            instancesTransform.setScale(instancesBounds.getSize() /
                                        modelBounds.getSize());

            addInstance(_rootModel, impl.getBoundingBoxModel(),
                        instancesTransform);
        }

        impl.logInformation();
    }

    BRAYNS_INFO << "Committing root models" << std::endl;
    ospCommit(_rootModel);
    if (_rootSimulationModel)
        ospCommit(_rootSimulationModel);

    _computeBounds();
}

bool OSPRayScene::commitLights()
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
                                   &_ospLights[0], _memoryManagementFlags);
        ospCommit(_ospLightData);
        for (auto renderer : _renderers)
        {
            auto impl =
                std::static_pointer_cast<OSPRayRenderer>(renderer)->impl();
            ospSetData(impl, "lights", _ospLightData);
        }
    }
    return true;
}

bool OSPRayScene::commitTransferFunctionData()
{
    if (!_transferFunction.isModified())
        return false;

    if (_ospTransferFunctionDiffuseData)
        ospRelease(_ospTransferFunctionDiffuseData);

    if (_ospTransferFunctionEmissionData)
        ospRelease(_ospTransferFunctionEmissionData);

    _ospTransferFunctionDiffuseData =
        ospNewData(_transferFunction.getDiffuseColors().size(), OSP_FLOAT4,
                   _transferFunction.getDiffuseColors().data(),
                   _memoryManagementFlags);
    ospCommit(_ospTransferFunctionDiffuseData);

    _ospTransferFunctionEmissionData =
        ospNewData(_transferFunction.getEmissionIntensities().size(),
                   OSP_FLOAT3,
                   _transferFunction.getEmissionIntensities().data(),
                   _memoryManagementFlags);
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
    _transferFunction.resetModified();
    return true;
}

bool OSPRayScene::commitVolumeData()
{
    const auto volumeHandler = getVolumeHandler();
    if (!volumeHandler)
        return false;

    const auto& vp = _parametersManager.getVolumeParameters();
    if (vp.isModified())
    {
        // Cleanup existing volume data in handler and renderers
        volumeHandler->clear();

        // An empty array has to be assigned to the renderers
        _ospVolumeDataSize = 0;
        _ospVolumeData = ospNewData(_ospVolumeDataSize, OSP_UCHAR, 0,
                                    _memoryManagementFlags);
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
        _ospVolumeData = ospNewData(_ospVolumeDataSize, OSP_UCHAR, data,
                                    _memoryManagementFlags);
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
    }
    return true;
}

bool OSPRayScene::commitSimulationData()
{
    if (!_simulationHandler)
        return false;

    const auto animationFrame =
        _parametersManager.getAnimationParameters().getFrame();

    if (_simulationHandler->getCurrentFrame() == animationFrame)
        return false;

    auto frameData = _simulationHandler->getFrameData(animationFrame);

    if (!frameData)
        return false;

    if (_ospSimulationData)
        ospRelease(_ospSimulationData);
    _ospSimulationData =
        ospNewData(_simulationHandler->getFrameSize(), OSP_FLOAT, frameData,
                   _memoryManagementFlags);
    ospCommit(_ospSimulationData);

    for (const auto& renderer : _renderers)
    {
        auto impl = std::static_pointer_cast<OSPRayRenderer>(renderer)->impl();
        ospSetData(impl, "simulationData", _ospSimulationData);
        ospSet1i(impl, "simulationDataSize",
                 _simulationHandler->getFrameSize());
    }
    return true;
}

bool OSPRayScene::isVolumeSupported(const std::string& volumeFile) const
{
    return boost::algorithm::ends_with(volumeFile, ".raw");
}

ModelPtr OSPRayScene::createModel() const
{
    return std::make_unique<OSPRayModel>();
}
}
