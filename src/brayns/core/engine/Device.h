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

#include <ospray/ospray.h>

#include <memory>
#include <stdexcept>

#include <brayns/core/utils/Logger.h>

namespace brayns
{
class DeviceException : public std::runtime_error
{
public:
    explicit DeviceException(OSPError error, const char *message);

    OSPError getError() const;

private:
    OSPError _error;
};

class DeviceErrorHandler
{
public:
    explicit DeviceErrorHandler(Logger &logger);

    void handle(DeviceException e);
    void throwIfError();

private:
    Logger *_logger;
    std::optional<DeviceException> _exception;
};

class Device
{
public:
    explicit Device(OSPDevice device, std::unique_ptr<DeviceErrorHandler> handler);

    OSPDevice getHandle() const;
    void throwIfError();

private:
    struct Deleter
    {
        void operator()(OSPDevice device) const;
    };

    std::unique_ptr<osp::Device, Deleter> _device;
    std::unique_ptr<DeviceErrorHandler> _handler;
};

Device createDevice(Logger &logger);
}
