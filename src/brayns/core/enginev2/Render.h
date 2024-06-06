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

#pragma once

#include "Camera.h"
#include "Framebuffer.h"
#include "Object.h"
#include "Renderer.h"
#include "World.h"

namespace brayns::experimental
{
struct RenderSettings
{
    Framebuffer framebuffer;
    Renderer renderer;
    Camera camera;
    World world;
};

class RenderTask : public Managed<OSPFuture>
{
public:
    using Managed::Managed;

    bool isReady() const;
    float getProgress() const;
    void cancel();
    float waitAndGetDuration();
};

struct PickSettings : RenderSettings
{
    Vector2 normalizedScreenPosition;
};

struct PickResult
{
    Vector3 worldPosition;
    Instance instance;
    GeometricModel model;
    std::uint32_t primitiveIndex;
};
}
