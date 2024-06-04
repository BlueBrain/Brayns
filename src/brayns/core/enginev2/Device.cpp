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

#include "Device.h"

namespace brayns::experimental
{
Device::Device(OSPDevice handle):
    _handle(handle)
{
}

GeometricModel Device::createGeometricModel()
{
    auto handle = ospNewGeometricModel();
    return GeometricModel(handle);
}

VolumetricModel Device::createVolumetricModel()
{
    auto handle = ospNewVolumetricModel();
    return VolumetricModel(handle);
}

Group Device::createGroup()
{
    auto handle = ospNewGroup();
    return Group(handle);
}

Instance Device::createInstance()
{
    auto handle = ospNewInstance();
    return Instance(handle);
}

World Device::createWorld()
{
    auto handle = ospNewWorld();
    return World(handle);
}

Framebuffer Device::createFramebuffer(const FramebufferSettings &settings)
{
    auto width = static_cast<int>(settings.width);
    auto height = static_cast<int>(settings.height);
    auto format = static_cast<OSPFrameBufferFormat>(settings.format);
    auto channels = static_cast<std::uint32_t>(OSP_FB_NONE);
    for (auto channel : settings.channels)
    {
        channels |= static_cast<OSPFrameBufferChannel>(channel);
    }
    auto handle = ospNewFrameBuffer(width, height, format, channels);
    return Framebuffer(handle);
}

RenderTask Device::render(const RenderSettings &settings)
{
    auto framebuffer = settings.framebuffer.getHandle();
    auto renderer = settings.renderer.getHandle();
    auto camera = settings.camera.getHandle();
    auto world = settings.world.getHandle();
    auto future = ospRenderFrame(framebuffer, renderer, camera, world);
    return RenderTask(future);
}

void Device::Deleter::operator()(OSPDevice device) const
{
    ospDeviceRelease(device);
    ospShutdown();
}

Device createDevice(Logger &logger)
{
    auto error = ospLoadModule("cpu");

    if (error != OSP_NO_ERROR)
    {
        auto message = fmt::format("Failed to load OSPRay CPU module (code = {})", static_cast<int>(error));
        throw std::runtime_error(message);
    }

    auto currentDevice = ospGetCurrentDevice();
    if (currentDevice != nullptr)
    {
        throw std::invalid_argument("OSPRay only accepts one device created at a time");
    }

    auto device = ospNewDevice();

    auto logLevel = OSP_LOG_DEBUG;
    ospDeviceSetParam(device, "logLevel", OSP_UINT, &logLevel);

    auto warnAsError = true;
    ospDeviceSetParam(device, "warnAsError", OSP_BOOL, &warnAsError);

    auto errorCallback = [](auto *userData, auto code, const auto *message)
    {
        auto &logger = *static_cast<Logger *>(userData);
        logger.error("Device error (code = {}): {}", static_cast<int>(code), message);
    };
    ospDeviceSetErrorCallback(device, errorCallback, &logger);

    auto statusCallback = [](auto *userData, const auto *message)
    {
        auto &logger = *static_cast<Logger *>(userData);
        logger.debug("Device status: {}", message);
    };
    ospDeviceSetStatusCallback(device, statusCallback, &logger);

    ospDeviceCommit(device);
    ospSetCurrentDevice(device);

    return Device(device);
}
}
