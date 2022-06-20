/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/engine/ospray/OsprayMathtypesTraits.h>

namespace
{
class LightParameterUpdater
{
public:
    static void update(const brayns::Light &light)
    {
        static const std::string colorParameter = "color";
        static const std::string intensityParameter = "intensity";
        static const std::string visibilityParameter = "visible";

        auto &color = light.getColor();
        auto intensity = light.getIntensity();
        auto isVisible = light.isVisible();

        auto &osprayLight = light.getOsprayLight();
        osprayLight.setParam(colorParameter, color);
        osprayLight.setParam(intensityParameter, intensity);
        osprayLight.setParam(visibilityParameter, isVisible);
    }
};
}

namespace brayns
{
Light::Light(const std::string &handleID)
    : _osprayLight(handleID)
{
}

void Light::setColor(const Vector3f &color) noexcept
{
    _updateValue(_color, color);
}

void Light::setIntensity(const float intensity) noexcept
{
    _updateValue(_intensity, intensity);
}

void Light::setVisible(const bool visible) noexcept
{
    _updateValue(_visible, visible);
}

const Vector3f &Light::getColor() const noexcept
{
    return _color;
}

float Light::getIntensity() const noexcept
{
    return _intensity;
}

bool Light::isVisible() const noexcept
{
    return _visible;
}

Bounds Light::computeBounds() const noexcept
{
    return {};
}

bool Light::commit()
{
    if (!isModified())
    {
        return false;
    }

    LightParameterUpdater::update(*this);

    commitLightSpecificParams();

    _osprayLight.commit();

    resetModified();

    return true;
}

const ospray::cpp::Light &Light::getOsprayLight() const noexcept
{
    return _osprayLight;
}
} // namespace brayns
