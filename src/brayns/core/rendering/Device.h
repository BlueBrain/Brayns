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

#include "Geometry.h"
#include "Light.h"
#include "Volume.h"

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

    StructuredRegularVolume createStructuredRegularVolume();

    MeshGeometry createMesh();
    SphereGeometry createSpheres();
    CurveGeometry createCurve();
    BoxGeometry createBoxes();
    PlaneGeometry createPlanes();
    IsosurfaceGeometry createIsoSurfaces();

    VolumeModel createVolumeModel(ospray::cpp::Volume volume);
    GeometryModel createGeometryModel(ospray::cpp::Geometry geometry);

    DistantLight createDistantLight();
    SphereLight createSphereLight();
    SpotLight createSpotLight();
    QuadLight createQuadLight();
    AmbientLight createAmbientLight();

private:
    ospray::cpp::Device _device;
};

Device createDevice(Logger &logger);
}
