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
#include "OSPRayVolume.h"
#include "utils.h"

#include <brayns/common/ImageManager.h>
#include <brayns/common/Transformation.h>
#include <brayns/common/light/DirectionalLight.h>
#include <brayns/common/light/PointLight.h>
#include <brayns/common/log.h>
#include <brayns/common/scene/Model.h>
#include <brayns/common/simulation/AbstractSimulationHandler.h>

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
    ospCommit(_ospTransferFunction);
}

OSPRayScene::~OSPRayScene()
{
    ospRelease(_ospTransferFunction);

    if (_ospSimulationData)
        ospRelease(_ospSimulationData);

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
    const bool rebuildScene = isModified();
    const bool addRemoveVolumes = _commitVolumeData();

    _commitSimulationData();
    commitTransferFunctionData();

    if (!rebuildScene && !addRemoveVolumes)
        return;

    _activeModels.clear();

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

        // keep models from being deleted via removeModel() as long as we use
        // them here
        _activeModels.push_back(modelDescriptor);

        auto& impl = static_cast<OSPRayModel&>(modelDescriptor->getModel());
        const auto& transformation = modelDescriptor->getTransformation();

        BRAYNS_DEBUG << "Committing " << modelDescriptor->getName()
                     << std::endl;

        if (modelDescriptor->getVisible() && impl.getUseSimulationModel())
        {
            if (!_rootSimulationModel)
                _rootSimulationModel = ospNewModel();
            addInstance(_rootSimulationModel, impl.getSimulationModel(),
                        transformation);
        }

        // add volumes to root model, because scivis renderer does not consider
        // volumes from instances
        if (modelDescriptor->getVisible())
        {
            for (auto volume : modelDescriptor->getModel().getVolumes())
            {
                auto ospVolume =
                    std::dynamic_pointer_cast<OSPRayVolume>(volume);
                ospAddVolume(_rootModel, ospVolume->impl());
            }
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
    BRAYNS_DEBUG << "Committing root models" << std::endl;
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

    // for volumes
    Vector3fs colors;
    colors.reserve(_transferFunction.getDiffuseColors().size());
    floats opacities;
    opacities.reserve(_transferFunction.getDiffuseColors().size());

    for (const auto& i : _transferFunction.getDiffuseColors())
    {
        colors.push_back({i.x(), i.y(), i.z()});
        opacities.push_back(i.w());
    }

    OSPData colorsData = ospNewData(colors.size(), OSP_FLOAT3, colors.data());
    ospSetData(_ospTransferFunction, "colors", colorsData);
    ospRelease(colorsData);
    ospSet2f(_ospTransferFunction, "valueRange",
             _transferFunction.getValuesRange().x(),
             _transferFunction.getValuesRange().y());
    OSPData opacityValuesData =
        ospNewData(opacities.size(), OSP_FLOAT, opacities.data());
    ospSetData(_ospTransferFunction, "opacities", opacityValuesData);
    ospRelease(opacityValuesData);
    ospCommit(_ospTransferFunction);

    // for simulation
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
    markModified();
    return true;
}

bool OSPRayScene::_commitVolumeData()
{
    bool rebuildScene = false;
    std::shared_lock<std::shared_timed_mutex> lock(_modelMutex);
    for (auto modelDescriptor : _modelDescriptors)
    {
        auto& model = modelDescriptor->getModel();
        if (model.isVolumesDirty())
        {
            rebuildScene = true;
            model.resetVolumesDirty();
        }
        for (auto volume : model.getVolumes())
        {
            if (volume->isModified() || rebuildScene)
            {
                volume->commit();
                markModified(); // to reset accumulation if new blocks are added
            }
        }
        modelDescriptor->getModel().updateSizeInBytes();
    }
    return rebuildScene;
}

void OSPRayScene::_commitSimulationData()
{
    if (!_simulationHandler)
        return;

    const auto animationFrame =
        _parametersManager.getAnimationParameters().getFrame();

    if (_ospSimulationData &&
        _simulationHandler->getCurrentFrame() == animationFrame)
    {
        return;
    }

    auto frameData = _simulationHandler->getFrameData(animationFrame);

    if (!frameData)
        return;

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
        ospCommit(impl);
    }
    markModified(); // triggers framebuffer clear
}

ModelPtr OSPRayScene::createModel() const
{
    return std::make_unique<OSPRayModel>();
}

SharedDataVolumePtr OSPRayScene::createSharedDataVolume(
    const Vector3ui& dimensions, const Vector3f& spacing,
    const DataType type) const
{
    return std::make_shared<OSPRaySharedDataVolume>(
        dimensions, spacing, type, _parametersManager.getVolumeParameters(),
        _ospTransferFunction);
}

BrickedVolumePtr OSPRayScene::createBrickedVolume(const Vector3ui& dimensions,
                                                  const Vector3f& spacing,
                                                  const DataType type) const
{
    return std::make_shared<OSPRayBrickedVolume>(
        dimensions, spacing, type, _parametersManager.getVolumeParameters(),
        _ospTransferFunction);
}
}
