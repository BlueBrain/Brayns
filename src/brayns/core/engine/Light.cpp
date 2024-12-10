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
    setObjectParam(handle, "visible", settings.visible);
}

void setIntensityValue(OSPLight handle, Power power)
{
    setObjectParam(handle, "intensityQuantity", OSP_INTENSITY_QUANTITY_POWER);
    setObjectParam(handle, "intensity", power.value);
}

void setIntensityValue(OSPLight handle, Intensity intensity)
{
    setObjectParam(handle, "intensityQuantity", OSP_INTENSITY_QUANTITY_INTENSITY);
    setObjectParam(handle, "intensity", intensity.value);
}

void setIntensityValue(OSPLight handle, Radiance radiance)
{
    setObjectParam(handle, "intensityQuantity", OSP_INTENSITY_QUANTITY_RADIANCE);
    setObjectParam(handle, "intensity", radiance.value);
}

void setIntensityValue(OSPLight handle, Irradiance irradiance)
{
    setObjectParam(handle, "intensityQuantity", OSP_INTENSITY_QUANTITY_IRRADIANCE);
    setObjectParam(handle, "intensity", irradiance.value);
}

void setIntensityScale(OSPLight handle, float value)
{
    setObjectParam(handle, "intensityQuantity", OSP_INTENSITY_QUANTITY_SCALE);
    setObjectParam(handle, "intensity", value);
}

void setIntensityParam(OSPLight handle, const auto &intensity)
{
    std::visit([&](const auto &value) { setIntensityValue(handle, value); }, intensity);
}

void setDistantLightParams(OSPLight handle, const DistantLightSettings &settings)
{
    setObjectParam(handle, "direction", settings.direction);
    setObjectParam(handle, "angularDiameter", degrees(settings.angularDiameter));
    setIntensityParam(handle, settings.intensity);
}

void setSphereLightParams(OSPLight handle, const SphereLightSettings &settings)
{
    setObjectParam(handle, "position", settings.position);
    setObjectParam(handle, "radius", settings.radius);
    setIntensityParam(handle, settings.intensity);
}

void setSpotLightParams(OSPLight handle, const SpotLightSettings &settings)
{
    setObjectParam(handle, "position", settings.position);
    setObjectParam(handle, "direction", settings.direction);
    setObjectParam(handle, "openingAngle", degrees(settings.openingAngle));
    setObjectParam(handle, "penumbraAngle", degrees(settings.penumbraAngle));
    setObjectParam(handle, "radius", settings.outerRadius);
    setObjectParam(handle, "innerRadius", settings.innerRadius);
    setIntensityParam(handle, settings.intensity);
}

void setQuadLightParams(OSPLight handle, const QuadLightSettings &settings)
{
    setObjectParam(handle, "position", settings.position);
    setObjectParam(handle, "edge1", settings.edge1);
    setObjectParam(handle, "edge2", settings.edge2);
    setIntensityParam(handle, settings.intensity);
}

void setCylinderLightParams(OSPLight handle, const CylinderLightSettings &settings)
{
    setObjectParam(handle, "position0", settings.start);
    setObjectParam(handle, "position1", settings.end);
    setObjectParam(handle, "radius", settings.radius);
    setIntensityParam(handle, settings.intensity);
}

void setHdriLightParams(OSPLight handle, const Texture2D &map, const HdriLightSettings &settings)
{
    setObjectParam(handle, "map", map);
    setObjectParam(handle, "up", settings.up);
    setObjectParam(handle, "direction", settings.direction);
    setIntensityScale(handle, settings.intensityScale);
}

void setAmbientLightParams(OSPLight handle, const AmbientLightSettings &settings)
{
    setIntensityParam(handle, settings.intensity);
}

void setSunSkyLightParams(OSPLight handle, const SunSkyLightSettings &settings)
{
    setObjectParam(handle, "up", settings.up);
    setObjectParam(handle, "direction", settings.direction);
    setObjectParam(handle, "turbidity", settings.turbidity);
    setObjectParam(handle, "albedo", settings.albedo);
    setObjectParam(handle, "horizonExtension", settings.horizonExtension);
    setIntensityScale(handle, settings.intensityScale);
}
}

namespace brayns
{
void Light::update(const LightSettings &settings)
{
    auto handle = getHandle();
    setLightParams(handle, settings);
    commitObject(handle);
}

void DistantLight::update(const DistantLightSettings &settings)
{
    auto handle = getHandle();
    setDistantLightParams(handle, settings);
    commitObject(handle);
}

DistantLight createDistantLight(Device &device, const LightSettings &settings, const DistantLightSettings &distant)
{
    auto handle = ospNewLight("distant");
    auto light = wrapObjectHandleAs<DistantLight>(device, handle);

    setLightParams(handle, settings);
    setDistantLightParams(handle, distant);

    commitObject(device, handle);

    return light;
}

void SphereLight::update(const SphereLightSettings &settings)
{
    auto handle = getHandle();
    setSphereLightParams(handle, settings);
    commitObject(handle);
}

SphereLight createSphereLight(Device &device, const LightSettings &settings, const SphereLightSettings &sphere)
{
    auto handle = ospNewLight("sphere");
    auto light = wrapObjectHandleAs<SphereLight>(device, handle);

    setLightParams(handle, settings);
    setSphereLightParams(handle, sphere);

    commitObject(device, handle);

    return light;
}

void SpotLight::update(const SpotLightSettings &settings)
{
    auto handle = getHandle();
    setSpotLightParams(handle, settings);
    commitObject(handle);
}

SpotLight createSpotLight(Device &device, const LightSettings &settings, const SpotLightSettings &spot)
{
    auto handle = ospNewLight("spot");
    auto light = wrapObjectHandleAs<SpotLight>(device, handle);

    setLightParams(handle, settings);
    setSpotLightParams(handle, spot);

    commitObject(device, handle);

    return light;
}

void QuadLight::update(const QuadLightSettings &settings)
{
    auto handle = getHandle();
    setQuadLightParams(handle, settings);
    commitObject(handle);
}

QuadLight createQuadLight(Device &device, const LightSettings &settings, const QuadLightSettings &quad)
{
    auto handle = ospNewLight("quad");
    auto light = wrapObjectHandleAs<QuadLight>(device, handle);

    setLightParams(handle, settings);
    setQuadLightParams(handle, quad);

    commitObject(device, handle);

    return light;
}

void CylinderLight::update(const CylinderLightSettings &settings)
{
    auto handle = getHandle();
    setCylinderLightParams(handle, settings);
    commitObject(handle);
}

CylinderLight createCylinderLight(Device &device, const LightSettings &settings, const CylinderLightSettings &cylinder)
{
    auto handle = ospNewLight("cylinder");
    auto light = wrapObjectHandleAs<CylinderLight>(device, handle);

    setLightParams(handle, settings);
    setCylinderLightParams(handle, cylinder);

    commitObject(device, handle);

    return light;
}

void HdriLight::update(const Texture2D &map, const HdriLightSettings &settings)
{
    auto handle = getHandle();
    setHdriLightParams(handle, map, settings);
    commitObject(handle);
}

HdriLight createHdriLight(Device &device, const Texture2D &map, const LightSettings &settings, const HdriLightSettings &hdri)
{
    auto handle = ospNewLight("hdri");
    auto light = wrapObjectHandleAs<HdriLight>(device, handle);

    setLightParams(handle, settings);
    setHdriLightParams(handle, map, hdri);

    commitObject(device, handle);

    return light;
}

void AmbientLight::update(const AmbientLightSettings &settings)
{
    auto handle = getHandle();
    setAmbientLightParams(handle, settings);
    commitObject(handle);
}

AmbientLight createAmbientLight(Device &device, const LightSettings &settings, const AmbientLightSettings &ambient)
{
    auto handle = ospNewLight("ambient");
    auto light = wrapObjectHandleAs<AmbientLight>(device, handle);

    setLightParams(handle, settings);
    setAmbientLightParams(handle, ambient);

    commitObject(device, handle);

    return light;
}

void SunSkyLight::update(const SunSkyLightSettings &settings)
{
    auto handle = getHandle();
    setSunSkyLightParams(handle, settings);
    commitObject(handle);
}

SunSkyLight createSunSkyLight(Device &device, const LightSettings &settings, const SunSkyLightSettings &sunsky)
{
    auto handle = ospNewLight("sunSky");
    auto light = wrapObjectHandleAs<SunSkyLight>(device, handle);

    setLightParams(handle, settings);
    setSunSkyLightParams(handle, sunsky);

    commitObject(device, handle);

    return light;
}
}
