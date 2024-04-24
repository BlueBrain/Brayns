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

#include <fmt/format.h>

namespace brayns
{
Device::Device(ospray::cpp::Device device):
    _device(std::move(device))
{
}

Device::~Device()
{
    if (_device.handle() != nullptr)
    {
        ospShutdown();
    }
}

GeometryModel Device::createGeometryModel()
{
    auto geometry = ospray::cpp::Geometry();
    auto handle = ospray::cpp::GeometricModel(geometry);
    return GeometryModel(std::move(handle));
}

VolumeModel brayns::Device::createVolumeModel()
{
    auto volume = ospray::cpp::Volume();
    auto handle = ospray::cpp::VolumetricModel(volume);
    return VolumeModel(std::move(handle));
}

FrameBuffer Device::createFramebuffer(const FramebufferSettings &settings)
{
    auto width = static_cast<int>(settings.width);
    auto height = static_cast<int>(settings.height);
    auto format = static_cast<OSPFrameBufferFormat>(settings.format);
    auto channels = static_cast<int>(OSP_FB_NONE);

    for (auto channel : settings.channels)
    {
        channels |= static_cast<OSPFrameBufferChannel>(channel);
    }

    auto handle = ospray::cpp::FrameBuffer(width, height, format, channels);

    return FrameBuffer(handle);
}

Device createDevice(Logger &logger)
{
    auto currentDevice = ospray::cpp::Device::current();
    if (currentDevice.handle() != nullptr)
    {
        throw std::invalid_argument("OSPRay only accepts one device created at a time");
    }

    auto error = ospLoadModule("cpu");
    if (error != OSP_NO_ERROR)
    {
        auto message = fmt::format("Failed to load CPU module (code = {})", static_cast<int>(error));
        throw std::runtime_error(message);
    }

    auto device = ospray::cpp::Device("cpu");

    device.setParam("logLevel", OSP_LOG_DEBUG);

    auto errorCallback = [](auto *userData, auto code, const auto *message)
    {
        auto &logger = *static_cast<Logger *>(userData);
        logger.error("Device error (code = {}): {}", static_cast<int>(code), message);
    };
    ospDeviceSetErrorCallback(device.handle(), errorCallback, &logger);

    auto statusCallback = [](auto *userData, const auto *message)
    {
        auto &logger = *static_cast<Logger *>(userData);
        logger.debug("Device status: {}", message);
    };
    ospDeviceSetStatusCallback(device.handle(), statusCallback, &logger);

    device.commit();
    device.setCurrent();

    return Device(std::move(device));
}
}
