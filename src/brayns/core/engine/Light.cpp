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

#include "Light.h"

namespace
{
using namespace brayns;

void setLightParams(OSPLight handle, const LightSettings &settings)
{
    setObjectParam(handle, "color", settings.color);
    setObjectParam(handle, "intensity", settings.intensity);
    setObjectParam(handle, "visible", settings.visible);
}
}

namespace brayns
{
DistantLight createDistantLight(Device &device, const LightSettings &settings, const DistantLightSettings &distant)
{
    auto handle = ospNewLight("distant");
    auto light = wrapObjectHandleAs<DistantLight>(device, handle);

    setLightParams(handle, settings);

    setObjectParam(handle, "direction", distant.direction);
    setObjectParam(handle, "angularDiameter", distant.angularDiameter);

    commitObject(device, handle);

    return light;
}

SphereLight createSphereLight(Device &device, const LightSettings &settings, const SphereLightSettings &sphere)
{
    auto handle = ospNewLight("sphere");
    auto light = wrapObjectHandleAs<SphereLight>(device, handle);

    setLightParams(handle, settings);

    setObjectParam(handle, "position", sphere.position);
    setObjectParam(handle, "radius", sphere.radius);

    commitObject(device, handle);

    return light;
}

SpotLight createSpotLight(Device &device, const LightSettings &settings, const SpotLightSettings &spot)
{
    auto handle = ospNewLight("spot");
    auto light = wrapObjectHandleAs<SpotLight>(device, handle);

    setLightParams(handle, settings);

    setObjectParam(handle, "position", spot.position);
    setObjectParam(handle, "direction", spot.direction);
    setObjectParam(handle, "openingAngle", spot.openingAngle);
    setObjectParam(handle, "penumbraAngle", spot.penumbraAngle);
    setObjectParam(handle, "radius", spot.outerRadius);
    setObjectParam(handle, "innerRadius", spot.innerRadius);

    commitObject(device, handle);

    return light;
}

QuadLight createQuadLight(Device &device, const LightSettings &settings, const QuadLightSettings &quad)
{
    auto handle = ospNewLight("quad");
    auto light = wrapObjectHandleAs<QuadLight>(device, handle);

    setLightParams(handle, settings);

    setObjectParam(handle, "position", quad.position);
    setObjectParam(handle, "edge1", quad.edge1);
    setObjectParam(handle, "edge2", quad.edge2);

    commitObject(device, handle);

    return light;
}

CylinderLight createCylinderLight(Device &device, const LightSettings &settings, const CylinderLightSettings &cylinder)
{
    auto handle = ospNewLight("cylinder");
    auto light = wrapObjectHandleAs<CylinderLight>(device, handle);

    setLightParams(handle, settings);

    setObjectParam(handle, "position0", cylinder.start);
    setObjectParam(handle, "position1", cylinder.end);
    setObjectParam(handle, "radius", cylinder.radius);

    commitObject(device, handle);

    return light;
}

HdriLight createHdriLight(Device &device, const LightSettings &settings, const HdriLightSettings &hdri)
{
    auto handle = ospNewLight("hdri");
    auto light = wrapObjectHandleAs<HdriLight>(device, handle);

    setLightParams(handle, settings);

    setObjectParam(handle, "up", hdri.up);
    setObjectParam(handle, "direction", hdri.direction);
    setObjectParam(handle, "map", hdri.map);

    commitObject(device, handle);

    return light;
}

AmbientLight createAmbientLight(Device &device, const LightSettings &settings)
{
    auto handle = ospNewLight("ambient");
    auto light = wrapObjectHandleAs<AmbientLight>(device, handle);

    setLightParams(handle, settings);

    commitObject(device, handle);

    return light;
}

SunSkyLight createSunSkyLight(Device &device, const LightSettings &settings, const SunSkyLightSettings &sunsky)
{
    auto handle = ospNewLight("sunSky");
    auto light = wrapObjectHandleAs<SunSkyLight>(device, handle);

    setLightParams(handle, settings);

    setObjectParam(handle, "up", sunsky.up);
    setObjectParam(handle, "direction", sunsky.direction);
    setObjectParam(handle, "turbidity", sunsky.turbidity);
    setObjectParam(handle, "albedo", sunsky.albedo);
    setObjectParam(handle, "horizonExtension", sunsky.horizonExtension);

    commitObject(device, handle);

    return light;
}
}
