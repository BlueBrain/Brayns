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

#include <ospray/ospray_cpp.h>

#include <brayns/core/utils/Logger.h>

#include "Camera.h"
#include "Framebuffer.h"
#include "Geometry.h"
#include "Light.h"
#include "Material.h"
#include "Render.h"
#include "Renderer.h"
#include "Volume.h"
#include "World.h"

namespace brayns
{
class Device
{
public:
    explicit Device(ospray::cpp::Device device);
    ~Device();

    Device(const Device &) = delete;
    Device(Device &&) = default;
    Device &operator=(const Device &) = delete;
    Device &operator=(Device &&) = default;

    GeometryModel createGeometryModel();
    VolumeModel createVolumeModel();
    Group createGroup();
    World createWorld();
    FrameBuffer createFramebuffer(const FramebufferSettings &settings);

    RenderTask render(const RenderSettings &settings);

    template<typename ObjectType>
    ObjectType create()
    {
        using HandleType = typename ObjectType::HandleType;
        auto &name = ObjectType::name;
        auto handle = HandleType(name);
        return ObjectType(std::move(handle));
    }

private:
    ospray::cpp::Device _device;
};

Device createDevice(Logger &logger);
}
