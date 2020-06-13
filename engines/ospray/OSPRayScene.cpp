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
#include "OSPRayVolume.h"
#include "utils.h"

#include <brayns/common/ImageManager.h>
#include <brayns/common/Transformation.h>
#include <brayns/common/light/Light.h>
#include <brayns/common/log.h>
#include <brayns/engineapi/Model.h>

#include <brayns/parameters/GeometryParameters.h>
#include <brayns/parameters/VolumeParameters.h>

namespace brayns
{
OSPRayScene::OSPRayScene(AnimationParameters& animationParameters,
                         GeometryParameters& geometryParameters,
                         VolumeParameters& volumeParameters)
    : Scene(animationParameters, geometryParameters, volumeParameters)
    , _memoryManagementFlags(geometryParameters.getMemoryMode() ==
                                     MemoryMode::shared
                                 ? uint32_t(OSP_DATA_SHARED_BUFFER)
                                 : 0)
{
    _backgroundMaterial = std::make_shared<OSPRayMaterial>(PropertyMap(), true);
}

OSPRayScene::~OSPRayScene()
{
    _destroyLights();
    if (_rootModel)
        ospRelease(_rootModel);
}
void OSPRayScene::_destroyLights()
{
    for (auto& light : _ospLights)
        ospRelease(light);
    _ospLights.clear();

    ospRelease(_ospLightData);
    _ospLightData = nullptr;
}

void OSPRayScene::commit()
{
    Scene::commit();
    commitLights();

    // copy the list to avoid locking the mutex
    ModelDescriptors modelDescriptors;
    {
        auto lock = acquireReadAccess();
        modelDescriptors = _modelDescriptors;
    }

    const bool rebuildScene = isModified();
    const bool addRemoveVolumes =
        _commitVolumeAndTransferFunction(modelDescriptors);

    if (!rebuildScene && !addRemoveVolumes)
    {
        // check for dirty models aka their geometry has been altered
        bool doUpdate = false;
        for (auto& modelDescriptor : modelDescriptors)
        {
            auto& model = modelDescriptor->getModel();
            if (model.isDirty())
            {
                model.commitGeometry();
                // need to continue re-adding the models to update the bounding
                // box model to reflect the new model size
                doUpdate = true;
            }
        }
        if (!doUpdate)
            return;
    }

    _activeModels.clear();

    if (_rootModel)
        ospRelease(_rootModel);
    _rootModel = ospNewModel();

    for (auto modelDescriptor : modelDescriptors)
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

        impl.commitGeometry();
        impl.logInformation();

        // add volumes to root model, because scivis renderer does not consider
        // volumes from instances
        if (modelDescriptor->getVisible())
        {
            modelDescriptor->getModel().commitGeometry();
            for (auto volume : modelDescriptor->getModel().getVolumes())
            {
                auto ospVolume =
                    std::dynamic_pointer_cast<OSPRayVolume>(volume);
                ospAddVolume(_rootModel, ospVolume->impl());
            }
        }

        const auto& instances = modelDescriptor->getInstances();
        for (size_t i = 0; i < instances.size(); ++i)
        {
            const auto& instance = instances[i];

            // First instance uses model transformation
            const auto& instanceTransform =
                (i == 0 ? transformation : instance.getTransformation());

            if (modelDescriptor->getBoundingBox() && instance.getBoundingBox())
            {
                // scale and move the unit-sized bounding box geometry to the
                // model size/scale first, then apply the instance transform
                const auto& modelBounds =
                    modelDescriptor->getModel().getBounds();
                Transformation modelTransform;
                modelTransform.setTranslation(modelBounds.getCenter() -
                                              0.5 * modelBounds.getSize());
                modelTransform.setScale(modelBounds.getSize());

                addInstance(_rootModel, impl.getBoundingBoxModel(),
                            transformationToAffine3f(instanceTransform) *
                                transformationToAffine3f(modelTransform));
            }

            if (modelDescriptor->getVisible() && instance.getVisible())
                addInstance(_rootModel, impl.getPrimaryModel(),
                            instanceTransform);
        }

        impl.markInstancesClean();
    }
    BRAYNS_DEBUG << "Committing root models" << std::endl;

    ospCommit(_rootModel);

    _computeBounds();
}

bool OSPRayScene::commitLights()
{
    if (!_lightManager.isModified())
        return false;

    _destroyLights();

    for (const auto& kv : _lightManager.getLights())
    {
        auto baseLight = kv.second;
        OSPLight ospLight{nullptr};

        switch (baseLight->_type)
        {
        case LightType::DIRECTIONAL:
        {
            ospLight = ospNewLight3("distant");
            const auto light = static_cast<DirectionalLight*>(baseLight.get());
            osphelper::set(ospLight, "direction", Vector3f(light->_direction));
            osphelper::set(ospLight, "angularDiameter",
                           static_cast<float>(light->_angularDiameter));
            break;
        }
        case LightType::SPHERE:
        {
            ospLight = ospNewLight3("point");
            const auto light = static_cast<SphereLight*>(baseLight.get());
            osphelper::set(ospLight, "position", Vector3f(light->_position));
            osphelper::set(ospLight, "radius",
                           static_cast<float>(light->_radius));
            break;
        }
        case LightType::QUAD:
        {
            ospLight = ospNewLight3("quad");
            const auto light = static_cast<QuadLight*>(baseLight.get());
            osphelper::set(ospLight, "position", Vector3f(light->_position));
            osphelper::set(ospLight, "edge1", Vector3f(light->_edge1));
            osphelper::set(ospLight, "edge2", Vector3f(light->_edge2));
            break;
        }
        case LightType::SPOTLIGHT:
        {
            ospLight = ospNewLight3("spot");
            const auto light = static_cast<SpotLight*>(baseLight.get());
            osphelper::set(ospLight, "position", Vector3f(light->_position));
            osphelper::set(ospLight, "direction", Vector3f(light->_direction));
            osphelper::set(ospLight, "openingAngle",
                           static_cast<float>(light->_openingAngle));
            osphelper::set(ospLight, "penumbraAngle",
                           static_cast<float>(light->_penumbraAngle));
            osphelper::set(ospLight, "radius",
                           static_cast<float>(light->_radius));
            break;
        }
        case LightType::AMBIENT:
        {
            ospLight = ospNewLight3("ambient");
            break;
        }
        }

        assert(ospLight);

        osphelper::set(ospLight, "color", Vector3f(baseLight->_color));
        osphelper::set(ospLight, "intensity",
                       static_cast<float>(baseLight->_intensity));
        osphelper::set(ospLight, "isVisible", baseLight->_isVisible);

        _ospLights.push_back(ospLight);
        ospCommit(ospLight);
    }

    // NOTE: since the lights are shared between scene and renderer we let
    // OSPRay allocate a new buffer to avoid use-after-free issues
    const size_t memoryFlags = 0;
    _ospLightData = ospNewData(_ospLights.size(), OSP_OBJECT, _ospLights.data(),
                               memoryFlags);
    ospCommit(_ospLightData);

    return true;
}

bool OSPRayScene::_commitVolumeAndTransferFunction(
    ModelDescriptors& modelDescriptors)
{
    bool rebuildScene = false;
    for (auto& modelDescriptor : modelDescriptors)
    {
        auto& model = static_cast<OSPRayModel&>(modelDescriptor->getModel());
        const bool dirtyTransferFunction = model.commitTransferFunction();
        const bool dirtySimulationData = model.commitSimulationData();

        if (dirtyTransferFunction || dirtySimulationData)
            markModified(false);
        if (model.isVolumesDirty())
        {
            rebuildScene = true;
            model.resetVolumesDirty();
        }
        for (auto volume : model.getVolumes())
        {
            if (volume->isModified() || rebuildScene ||
                _volumeParameters.isModified())
            {
                volume->commit();
                // to reset accumulation if new blocks are added
                markModified(false);
            }
        }
    }
    return rebuildScene;
}

ModelPtr OSPRayScene::createModel() const
{
    return std::make_unique<OSPRayModel>(_animationParameters,
                                         _volumeParameters);
}

ModelDescriptorPtr OSPRayScene::getSimulatedModel()
{
    auto lock = acquireReadAccess();
    for (auto model : _modelDescriptors)
    {
        const auto& ospModel =
            static_cast<const OSPRayModel&>(model->getModel());
        if (ospModel.simulationData())
            return model;
    }
    return ModelDescriptorPtr{};
}
} // namespace brayns
