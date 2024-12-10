/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include "RenderEndpoints.h"

#include <brayns/core/engine/Render.h>

#include <brayns/core/objects/CameraObjects.h>
#include <brayns/core/objects/FramebufferObjects.h>
#include <brayns/core/objects/RendererObjects.h>
#include <brayns/core/objects/SceneObjects.h>

namespace brayns
{
struct RenderParams
{
    ObjectId framebuffer;
    ObjectId renderer;
    ObjectId camera;
    ObjectId world;
};

template<>
struct JsonObjectReflector<RenderParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<RenderParams>();
        builder.field("framebuffer", [](auto &object) { return &object.framebuffer; }).description("ID of the framebuffer to store rendered pixels");
        builder.field("renderer", [](auto &object) { return &object.renderer; }).description("ID of the renderer to use for render");
        builder.field("camera", [](auto &object) { return &object.camera; }).description("ID of the camera to use for render");
        builder.field("world", [](auto &object) { return &object.world; }).description("ID of the world to render");
        return builder.build();
    }
};

struct RenderResult
{
    float duration;
};

template<>
struct JsonObjectReflector<RenderResult>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<RenderResult>();
        builder.field("duration", [](auto &object) { return &object.duration; }).description("Render duration in seconds");
        return builder.build();
    }
};

RenderResult render(ObjectManager &objects, Device &device, const RenderParams &params)
{
    auto framebuffer = objects.getAs<UserFramebuffer>(params.framebuffer);
    auto renderer = objects.getAs<UserRenderer>(params.renderer);
    auto camera = objects.getAs<UserCamera>(params.camera);
    auto world = objects.getAs<UserWorld>(params.world);

    auto settings = RenderSettings{
        .framebuffer = framebuffer.value,
        .renderer = renderer.get(),
        .camera = camera.get(),
        .world = world.value,
    };

    auto future = render(device, settings);
    auto duration = future.waitAndGetDuration();

    return {duration};
}

void addRenderEndpoints(ApiBuilder &builder, ObjectManager &objects, Device &device)
{
    builder.endpoint("render", [&](RenderParams params) { return render(objects, device, params); }).description("Render synchronously");
}
}
