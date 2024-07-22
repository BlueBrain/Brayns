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
DistantLight createDistantLight(Device &device, const DistantLightSettings &settings)
{
    auto handle = ospNewLight("distant");
    auto light = wrapObjectHandleAs<DistantLight>(device, handle);

    setLightParams(handle, settings.base);

    setObjectParam(handle, "direction", settings.direction);
    setObjectParam(handle, "angularDiameter", settings.angularDiameter);

    commitObject(handle);

    return light;
}

SphereLight createSphereLight(Device &device, const SphereLightSettings &settings)
{
    auto handle = ospNewLight("sphere");
    auto light = wrapObjectHandleAs<SphereLight>(device, handle);

    setLightParams(handle, settings.base);

    setObjectParam(handle, "position", settings.position);
    setObjectParam(handle, "radius", settings.radius);

    commitObject(handle);

    return light;
}

SpotLight createSphereLight(Device &device, const SpotLightSettings &settings)
{
    auto handle = ospNewLight("spot");
    auto light = wrapObjectHandleAs<SpotLight>(device, handle);

    setLightParams(handle, settings.base);

    setObjectParam(handle, "position", settings.position);
    setObjectParam(handle, "direction", settings.direction);
    setObjectParam(handle, "openingAngle", settings.openingAngle);
    setObjectParam(handle, "penumbraAngle", settings.penumbraAngle);
    setObjectParam(handle, "radius", settings.outerRadius);
    setObjectParam(handle, "innerRadius", settings.innerRadius);

    commitObject(handle);

    return light;
}

QuadLight createQuadLight(Device &device, const QuadLightSettings &settings)
{
    auto handle = ospNewLight("quad");
    auto light = wrapObjectHandleAs<QuadLight>(device, handle);

    setLightParams(handle, settings.base);

    setObjectParam(handle, "position", settings.position);
    setObjectParam(handle, "edge1", settings.edge1);
    setObjectParam(handle, "edge2", settings.edge2);

    commitObject(handle);

    return light;
}

CylinderLight createCylinderLight(Device &device, const CylinderLightSettings &settings)
{
    auto handle = ospNewLight("cylinder");
    auto light = wrapObjectHandleAs<CylinderLight>(device, handle);

    setLightParams(handle, settings.base);

    setObjectParam(handle, "position1", settings.start);
    setObjectParam(handle, "position2", settings.end);
    setObjectParam(handle, "radius", settings.radius);

    commitObject(handle);

    return light;
}

HdriLight createHdriLight(Device &device, const HdriLightSettings &settings)
{
    auto handle = ospNewLight("hdri");
    auto light = wrapObjectHandleAs<HdriLight>(device, handle);

    setLightParams(handle, settings.base);

    setObjectParam(handle, "up", settings.up);
    setObjectParam(handle, "direction", settings.direction);
    setObjectParam(handle, "map", settings.map);

    commitObject(handle);

    return light;
}

AmbientLight createAmbientLight(Device &device, const AmbientLightSettings &settings)
{
    auto handle = ospNewLight("ambient");
    auto light = wrapObjectHandleAs<AmbientLight>(device, handle);

    setLightParams(handle, settings.base);

    commitObject(handle);

    return light;
}

SunSkyLight createSunSkyLight(Device &device, const SunSkyLightSettings &settings)
{
    auto handle = ospNewLight("sunSky");
    auto light = wrapObjectHandleAs<SunSkyLight>(device, handle);

    setLightParams(handle, settings.base);

    setObjectParam(handle, "up", settings.up);
    setObjectParam(handle, "direction", settings.direction);
    setObjectParam(handle, "turbidity", settings.turbidity);
    setObjectParam(handle, "albedo", settings.albedo);
    setObjectParam(handle, "horizonExtension", settings.horizonExtension);

    commitObject(handle);

    return light;
}
}
