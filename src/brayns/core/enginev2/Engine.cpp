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

#include "Engine.h"

#include <fmt/format.h>

namespace brayns::experimental
{
Engine::Loader::~Loader()
{
    ospShutdown();
}

Engine::Engine(std::unique_ptr<Loader> loader):
    _loader(std::move(loader))
{
}

Device Engine::createDevice(Logger &logger)
{
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

Engine loadEngine()
{
    auto error = ospLoadModule("cpu");

    if (error != OSP_NO_ERROR)
    {
        auto message = fmt::format("Failed to load OSPRay CPU module (code = {})", static_cast<int>(error));
        throw std::runtime_error(message);
    }

    auto loader = std::make_unique<Engine::Loader>();

    return Engine(std::move(loader));
}
}
