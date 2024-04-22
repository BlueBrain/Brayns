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

StructuredRegularVolume Device::createStructuredRegularVolume()
{
    auto volume = ospray::cpp::Volume("structuredRegular");
    return StructuredRegularVolume(std::move(volume));
}

MeshGeometry Device::createMesh()
{
    auto geometry = ospray::cpp::Geometry("mesh");
    return MeshGeometry(std::move(geometry));
}

SphereGeometry Device::createSpheres()
{
    auto geometry = ospray::cpp::Geometry("sphere");
    return SphereGeometry(std::move(geometry));
}

CurveGeometry Device::createCurve()
{
    auto geometry = ospray::cpp::Geometry("curve");
    return CurveGeometry(std::move(geometry));
}

BoxGeometry Device::createBoxes()
{
    auto geometry = ospray::cpp::Geometry("box");
    return BoxGeometry(std::move(geometry));
}

PlaneGeometry Device::createPlanes()
{
    auto geometry = ospray::cpp::Geometry("plane");
    return PlaneGeometry(std::move(geometry));
}

IsosurfaceGeometry Device::createIsoSurfaces()
{
    auto geometry = ospray::cpp::Geometry("isosurface");
    return IsosurfaceGeometry(std::move(geometry));
}

VolumeModel Device::createVolumeModel(ospray::cpp::Volume volume)
{
    auto model = ospray::cpp::VolumetricModel(volume);
    return VolumeModel(std::move(model));
}

GeometryModel Device::createGeometryModel(ospray::cpp::Geometry geometry)
{
    auto model = ospray::cpp::GeometricModel(geometry);
    return GeometryModel(std::move(model));
}

DistantLight Device::createDistantLight()
{
    auto light = ospray::cpp::Light("distant");
    return DistantLight(std::move(light));
}

SphereLight Device::createSphereLight()
{
    auto light = ospray::cpp::Light("sphere");
    return SphereLight(std::move(light));
}

SpotLight Device::createSpotLight()
{
    auto light = ospray::cpp::Light("spot");
    return SpotLight(std::move(light));
}

QuadLight Device::createQuadLight()
{
    auto light = ospray::cpp::Light("quad");
    return QuadLight(std::move(light));
}

AmbientLight Device::createAmbientLight()
{
    auto light = ospray::cpp::Light("ambient");
    return AmbientLight(std::move(light));
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
