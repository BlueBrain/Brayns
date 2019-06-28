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

#include "OptiXRenderer.h"
#include "OptiXContext.h"
#include "OptiXFrameBuffer.h"
#include "OptiXMaterial.h"
#include "OptiXModel.h"
#include "OptiXScene.h"

#include <chrono>

using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

namespace
{
void toOptiXProperties(const brayns::PropertyMap& object)
{
    try
    {
        auto context = brayns::OptiXContext::get().getOptixContext();
        for (const auto& prop : object.getProperties())
        {
            switch (prop->type)
            {
            case brayns::Property::Type::Double:
                context[prop->name]->setFloat(
                    static_cast<float>(prop->get<double>()));
                break;
            case brayns::Property::Type::Int:
                context[prop->name]->setInt(prop->get<int32_t>());
                break;
            case brayns::Property::Type::Bool:
                // Special case, no bool in OptiX
                context[prop->name]->setUint(prop->get<bool>());
                break;
            case brayns::Property::Type::String:
                BRAYNS_WARN << "Cannot upload string property to OptiX '"
                            << prop->name << "'" << std::endl;
                break;
            case brayns::Property::Type::Vec2d:
            {
                auto v = prop->get<std::array<double, 2>>();
                context[prop->name]->setFloat(static_cast<float>(v[0]),
                                              static_cast<float>(v[1]));
                break;
            }
            case brayns::Property::Type::Vec2i:
            {
                auto v = prop->get<std::array<int32_t, 2>>();
                context[prop->name]->setInt(v[0], v[1]);
                break;
            }
            case brayns::Property::Type::Vec3d:
            {
                auto v = prop->get<std::array<double, 3>>();
                context[prop->name]->setFloat(static_cast<float>(v[0]),
                                              static_cast<float>(v[1]),
                                              static_cast<float>(v[2]));
                break;
            }
            case brayns::Property::Type::Vec3i:
            {
                auto v = prop->get<std::array<int32_t, 3>>();
                context[prop->name]->setInt(v[0], v[1], v[2]);
                break;
            }
            case brayns::Property::Type::Vec4d:
            {
                auto v = prop->get<std::array<double, 4>>();
                context[prop->name]->setFloat(static_cast<float>(v[0]),
                                              static_cast<float>(v[1]),
                                              static_cast<float>(v[2]),
                                              static_cast<float>(v[3]));
                break;
            }
            }
        }
    }
    catch (const std::exception& e)
    {
        BRAYNS_ERROR << "Failed to apply properties for OptiX object"
                     << e.what() << std::endl;
    }
}
}

namespace brayns
{
OptiXRenderer::OptiXRenderer(const AnimationParameters& animationParameters,
                             const RenderingParameters& renderingParameters)
    : Renderer(animationParameters, renderingParameters)
{
}

void OptiXRenderer::render(FrameBufferPtr frameBuffer)
{
    if (!frameBuffer->getAccumulation())
        return;

    // Provide a random seed to the renderer
    optix::float4 jitter = {(float)rand() / (float)RAND_MAX,
                            (float)rand() / (float)RAND_MAX,
                            (float)rand() / (float)RAND_MAX,
                            (float)rand() / (float)RAND_MAX};
    auto context = OptiXContext::get().getOptixContext();
    context["jitter4"]->setFloat(jitter);

    // Render
    frameBuffer->map();
    const auto size = frameBuffer->getSize();
    context->launch(0, size.x, size.y);
    frameBuffer->unmap();

    frameBuffer->markModified();
}

void OptiXRenderer::commit()
{
    const bool rendererChanged =
        _renderingParameters.getCurrentRenderer() != _currentRenderer;

    const bool updateMaterials =
        isModified() || rendererChanged || _scene->isModified();

    // If renderer or scene has changed we have to go through all materials in
    // the scene and update the renderer.
    if (updateMaterials)
    {
        const auto renderProgram = OptiXContext::get().getRenderer(
            _renderingParameters.getCurrentRenderer());

        _scene->visitModels([&](Model& model) {
            for (const auto& kv : model.getMaterials())
            {
                auto optixMaterial =
                    dynamic_cast<OptiXMaterial*>(kv.second.get());
                const bool textured = optixMaterial->isTextured();

                optixMaterial->getOptixMaterial()->setClosestHitProgram(
                    0, textured ? renderProgram->closest_hit_textured
                                : renderProgram->closest_hit);
                optixMaterial->getOptixMaterial()->setAnyHitProgram(
                    1, renderProgram->any_hit);
            }
        });
    }

    // Upload common properties
    auto context = OptiXContext::get().getOptixContext();
    auto bgColor = _renderingParameters.getBackgroundColor();
    const auto samples_per_pixel = _renderingParameters.getSamplesPerPixel();
    constexpr auto epsilon = 1.0e-5f;

    context["radianceRayType"]->setUint(0);
    context["shadowRayType"]->setUint(1);
    context["sceneEpsilon"]->setFloat(epsilon);
    context["ambientLightColor"]->setFloat(bgColor.x, bgColor.y, bgColor.z);
    context["bgColor"]->setFloat(bgColor.x, bgColor.y, bgColor.z);
    context["samples_per_pixel"]->setUint(samples_per_pixel);

    toOptiXProperties(getPropertyMap());
    _currentRenderer = _renderingParameters.getCurrentRenderer();
}

void OptiXRenderer::setCamera(CameraPtr /*camera*/)
{
}
} // namespace brayns
