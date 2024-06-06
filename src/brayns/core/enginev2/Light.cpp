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
void ObjectReflector<DistantLight>::loadParams(OSPLight handle, const Settings &settings)
{
    setLightParams(handle, settings);
    setObjectParam(handle, "direction", settings.direction);
    setObjectParam(handle, "angularDiameter", settings.angularDiameter);
    commitObject(handle);
}

void ObjectReflector<SphereLight>::loadParams(OSPLight handle, const Settings &settings)
{
    setLightParams(handle, settings);
    setObjectParam(handle, "position", settings.position);
    setObjectParam(handle, "radius", settings.radius);
    commitObject(handle);
}

void ObjectReflector<SpotLight>::loadParams(OSPLight handle, const Settings &settings)
{
    setLightParams(handle, settings);
    setObjectParam(handle, "position", settings.position);
    setObjectParam(handle, "direction", settings.direction);
    setObjectParam(handle, "openingAngle", settings.openingAngle);
    setObjectParam(handle, "penumbraAngle", settings.penumbraAngle);
    setObjectParam(handle, "radius", settings.outerRadius);
    setObjectParam(handle, "innerRadius", settings.innerRadius);
    commitObject(handle);
}

void ObjectReflector<QuadLight>::loadParams(OSPLight handle, const Settings &settings)
{
    setLightParams(handle, settings);
    setObjectParam(handle, "position", settings.position);
    setObjectParam(handle, "edge1", settings.edge1);
    setObjectParam(handle, "edge2", settings.edge2);
    commitObject(handle);
}

void ObjectReflector<AmbientLight>::loadParams(OSPLight handle, const Settings &settings)
{
    setLightParams(handle, settings);
    commitObject(handle);
}
}
