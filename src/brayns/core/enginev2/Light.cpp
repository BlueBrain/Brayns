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
using namespace brayns::experimental;

void setLightParams(OSPLight handle, const LightSettings &settings)
{
    setObjectParam(handle, "color", settings.color);
    setObjectParam(handle, "intensity", settings.intensity);
    setObjectParam(handle, "visible", settings.visible);
}
}

namespace brayns::experimental
{
OSPLight ObjectReflector<DistantLight>::createHandle(OSPDevice device, const Settings &settings)
{
    auto handle = ospNewLight("distant");
    throwLastDeviceErrorIfNull(device, handle);

    setLightParams(handle, settings.base);

    setObjectParam(handle, "direction", settings.direction);
    setObjectParam(handle, "angularDiameter", settings.angularDiameter);

    commitObject(handle);

    return handle;
}

OSPLight ObjectReflector<SphereLight>::createHandle(OSPDevice device, const Settings &settings)
{
    auto handle = ospNewLight("sphere");
    throwLastDeviceErrorIfNull(device, handle);

    setLightParams(handle, settings.base);

    setObjectParam(handle, "position", settings.position);
    setObjectParam(handle, "radius", settings.radius);

    commitObject(handle);

    return handle;
}

OSPLight ObjectReflector<SpotLight>::createHandle(OSPDevice device, const Settings &settings)
{
    auto handle = ospNewLight("spot");
    throwLastDeviceErrorIfNull(device, handle);

    setLightParams(handle, settings.base);

    setObjectParam(handle, "position", settings.position);
    setObjectParam(handle, "direction", settings.direction);
    setObjectParam(handle, "openingAngle", settings.openingAngle);
    setObjectParam(handle, "penumbraAngle", settings.penumbraAngle);
    setObjectParam(handle, "radius", settings.outerRadius);
    setObjectParam(handle, "innerRadius", settings.innerRadius);

    commitObject(handle);

    return handle;
}

OSPLight ObjectReflector<QuadLight>::createHandle(OSPDevice device, const Settings &settings)
{
    auto handle = ospNewLight("quad");
    throwLastDeviceErrorIfNull(device, handle);

    setLightParams(handle, settings.base);

    setObjectParam(handle, "position", settings.position);
    setObjectParam(handle, "edge1", settings.edge1);
    setObjectParam(handle, "edge2", settings.edge2);

    commitObject(handle);

    return handle;
}

OSPLight ObjectReflector<CylinderLight>::createHandle(OSPDevice device, const Settings &settings)
{
    auto handle = ospNewLight("cylinder");
    throwLastDeviceErrorIfNull(device, handle);

    setLightParams(handle, settings.base);

    setObjectParam(handle, "position1", settings.start);
    setObjectParam(handle, "position2", settings.end);
    setObjectParam(handle, "radius", settings.radius);

    commitObject(handle);

    return handle;
}

OSPLight ObjectReflector<HdriLight>::createHandle(OSPDevice device, const Settings &settings)
{
    auto handle = ospNewLight("hdri");
    throwLastDeviceErrorIfNull(device, handle);

    setLightParams(handle, settings.base);

    setObjectParam(handle, "up", settings.up);
    setObjectParam(handle, "direction", settings.direction);
    setObjectParam(handle, "map", settings.map);

    commitObject(handle);

    return handle;
}

OSPLight ObjectReflector<AmbientLight>::createHandle(OSPDevice device, const Settings &settings)
{
    auto handle = ospNewLight("ambient");
    throwLastDeviceErrorIfNull(device, handle);

    setLightParams(handle, settings.base);

    commitObject(handle);

    return handle;
}

OSPLight ObjectReflector<SunSkyLight>::createHandle(OSPDevice device, const Settings &settings)
{
    auto handle = ospNewLight("sunSky");
    throwLastDeviceErrorIfNull(device, handle);

    setLightParams(handle, settings.base);

    setObjectParam(handle, "up", settings.up);
    setObjectParam(handle, "direction", settings.direction);
    setObjectParam(handle, "turbidity", settings.turbidity);
    setObjectParam(handle, "albedo", settings.albedo);
    setObjectParam(handle, "horizonExtension", settings.horizonExtension);

    commitObject(handle);

    return handle;
}
}
