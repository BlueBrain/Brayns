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

namespace brayns
{
DeviceException::DeviceException(OSPError error, const char *message):
    std::runtime_error(message),
    _error(error)
{
}

OSPError DeviceException::getError() const
{
    return _error;
}

DeviceErrorHandler::DeviceErrorHandler(Logger &logger):
    _logger(&logger)
{
}

void DeviceErrorHandler::handle(DeviceException e)
{
    _logger->error("Device error (code = {}): {}", static_cast<int>(e.getError()), e.what());
    _exception = std::move(e);
}

void DeviceErrorHandler::throwIfError()
{
    if (!_exception)
    {
        return;
    }

    auto e = std::move(*_exception);

    _exception.reset();

    throw e;
}

Device::Device(OSPDevice device, std::unique_ptr<DeviceErrorHandler> handler):
    _device(device),
    _handler(std::move(handler))
{
}

OSPDevice Device::getHandle() const
{
    return _device.get();
}

void Device::throwIfError()
{
    _handler->throwIfError();
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

    if (device == nullptr)
    {
        throw std::runtime_error("Failed to create device");
    }

    auto handler = std::make_unique<DeviceErrorHandler>(logger);

    auto logLevel = OSP_LOG_DEBUG;
    ospDeviceSetParam(device, "logLevel", OSP_UINT, &logLevel);

    auto warnAsError = true;
    ospDeviceSetParam(device, "warnAsError", OSP_BOOL, &warnAsError);

    auto errorCallback = [](auto *userData, auto error, const auto *message)
    {
        auto &handler = *static_cast<DeviceErrorHandler *>(userData);
        handler.handle(DeviceException(error, message));
    };
    ospDeviceSetErrorCallback(device, errorCallback, handler.get());

    auto statusCallback = [](auto *userData, const auto *message)
    {
        auto &logger = *static_cast<Logger *>(userData);
        logger.debug("Device status: {}", message);
    };
    ospDeviceSetStatusCallback(device, statusCallback, &logger);

    ospDeviceCommit(device);
    ospSetCurrentDevice(device);

    return Device(device, std::move(handler));
}
}
