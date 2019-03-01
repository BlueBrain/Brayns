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

#include "OptiXScene.h"
#include "OptiXContext.h"
#include "OptiXMaterial.h"
#include "OptiXModel.h"

#include <brayns/common/light/DirectionalLight.h>
#include <brayns/common/light/PointLight.h>
#include <brayns/common/log.h>
#include <brayns/engine/Material.h>
#include <brayns/parameters/ParametersManager.h>

#include <optixu/optixu_math_stream_namespace.h>

namespace brayns
{
OptiXScene::OptiXScene(AnimationParameters& animationParameters,
                       GeometryParameters& geometryParameters,
                       VolumeParameters& volumeParameters)
    : Scene(animationParameters, geometryParameters, volumeParameters)
    , _lightBuffer(nullptr)
{
    _backgroundMaterial = std::make_shared<OptiXMaterial>();
    auto oc = OptiXContext::get().getOptixContext();

    // To avoid crashes we need to initialize some buffers and variables
    // even if they are not always used in CUDA kernels.

    { // Create dummy texture sampler
        ::optix::TextureSampler sampler = oc->createTextureSampler();
        sampler->setArraySize(1u);
        optix::Buffer buffer =
            oc->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT4, 1, 1);
        sampler->setBuffer(buffer);
        _dummyTextureSampler = sampler;
    }

    // Create dummy simulation data
    oc["simulation_data"]->setBuffer(
        oc->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT, 0));
}

OptiXScene::~OptiXScene() = default;

bool OptiXScene::commitLights()
{
    if (_lights.empty())
    {
        BRAYNS_ERROR << "No lights are currently defined" << std::endl;
        return false;
    }

    _optixLights.clear();
    for (auto light : _lights)
    {
        PointLight* pointLight = dynamic_cast<PointLight*>(light.get());
        if (pointLight != 0)
        {
            const Vector3f& position = pointLight->getPosition();
            const Vector3f& color = pointLight->getColor();
            BasicLight optixLight = {{position.x, position.y, position.z},
                                     {color.x, color.y, color.z},
                                     1, // Casts shadows
                                     BASIC_LIGHT_TYPE_POINT};
            _optixLights.push_back(optixLight);
        }
        else
        {
            DirectionalLight* directionalLight =
                dynamic_cast<DirectionalLight*>(light.get());
            if (directionalLight)
            {
                const Vector3f& direction = directionalLight->getDirection();
                const Vector3f& color = directionalLight->getColor();
                BasicLight optixLight = {{direction.x, direction.y,
                                          direction.z},
                                         {color.x, color.y, color.z},
                                         1, // Casts shadows
                                         BASIC_LIGHT_TYPE_DIRECTIONAL};
                _optixLights.push_back(optixLight);
            }
        }
    }

    if (_lightBuffer)
        _lightBuffer->destroy();

    auto context = OptiXContext::get().getOptixContext();
    _lightBuffer = context->createBuffer(RT_BUFFER_INPUT);
    _lightBuffer->setFormat(RT_FORMAT_USER);
    _lightBuffer->setElementSize(sizeof(BasicLight));
    _lightBuffer->setSize(_optixLights.size());
    memcpy(_lightBuffer->map(), _optixLights.data(),
           _optixLights.size() * sizeof(_optixLights[0]));
    _lightBuffer->unmap();
    context["lights"]->set(_lightBuffer);

    return true;
}

ModelPtr OptiXScene::createModel() const
{
    return std::make_unique<OptiXModel>(_animationParameters,
                                        _volumeParameters);
}

void OptiXScene::commit()
{
    // Always upload transfer function and simulation data if changed
    for (size_t i = 0; i < _modelDescriptors.size(); ++i)
    {
        auto& model = _modelDescriptors[i]->getModel();
        model.commitTransferFunction();
        model.commitSimulationData();
    }

    if (!isModified())
        return;

    // Remove all models marked for removal
    for (auto& model : _modelDescriptors)
        if (model->isMarkedForRemoval())
            model->callOnRemoved();

    _modelDescriptors.erase(
        std::remove_if(_modelDescriptors.begin(), _modelDescriptors.end(),
                       [](const auto& m) { return m->isMarkedForRemoval(); }),
        _modelDescriptors.end());

    auto context = OptiXContext::get().getOptixContext();

    if (hasEnvironmentMap())
    {
        auto texture = _backgroundMaterial->getTexture(TextureType::TT_DIFFUSE);
        if (_backgroundTextureSampler)
            _backgroundTextureSampler->destroy();
        _backgroundTextureSampler =
            OptiXContext::get().createTextureSampler(texture);
    }

    context["envmap"]->setTextureSampler(
        hasEnvironmentMap() ? _backgroundTextureSampler : _dummyTextureSampler);
    context["use_envmap"]->setUint(hasEnvironmentMap() ? 1 : 0);

    // Geometry
    if (_rootGroup)
        _rootGroup->destroy();

    _rootGroup = OptiXContext::get().createGroup();

    for (size_t i = 0; i < _modelDescriptors.size(); ++i)
    {
        auto& modelDescriptor = _modelDescriptors[i];
        if (!modelDescriptor->getEnabled())
            continue;

        auto& impl = static_cast<OptiXModel&>(modelDescriptor->getModel());

        BRAYNS_DEBUG << "Committing " << modelDescriptor->getName()
                     << std::endl;

        impl.commitGeometry();
        impl.logInformation();

        if (modelDescriptor->getVisible())
        {
            const auto geometryGroup = impl.getGeometryGroup();
            ::optix::Transform xform = context->createTransform();
            xform->setMatrix(false, ::optix::Matrix4x4::identity().getData(),
                             0); // TODO
            xform->setChild(geometryGroup);
            _rootGroup->addChild(xform);
            BRAYNS_DEBUG << "Group has " << geometryGroup->getChildCount()
                         << " children" << std::endl;
        }

        if (modelDescriptor->getBoundingBox())
        {
            // scale and move the unit-sized bounding box geometry to the
            // model size/scale first, then apply the instance transform
            const auto boundingBoxGroup = impl.getBoundingBoxGroup();
            ::optix::Transform xform = context->createTransform();

            const auto& modelBounds = modelDescriptor->getModel().getBounds();
            Transformation modelTransform;
            modelTransform.setTranslation(modelBounds.getCenter() /
                                              modelBounds.getSize() -
                                          Vector3d(0.5));
            modelTransform.setScale(modelBounds.getSize());

            Matrix4f mtxd = modelTransform.toMatrix(true);
            mtxd = glm::transpose(mtxd);
            auto trf = glm::value_ptr(mtxd);

            xform->setMatrix(false, trf, 0);
            xform->setChild(boundingBoxGroup);
            _rootGroup->addChild(xform);
        }
    }
    _computeBounds();

    BRAYNS_DEBUG << "Root has " << _rootGroup->getChildCount() << " children"
                 << std::endl;

    context["top_object"]->set(_rootGroup);
    context["top_shadower"]->set(_rootGroup);

    // TODO: triggers the change callback to re-broadcast the scene if the clip
    // planes have changed. Provide an RPC to update/set clip planes.
    markModified();
}

} // namespace brayns
