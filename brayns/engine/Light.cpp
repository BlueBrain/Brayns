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

#include <brayns/engine/Light.h>

namespace brayns
{
Light::Light(const Light &o)
{
    *this = o;
}

Light &Light::operator=(const Light &o)
{
    _color = o._color;
    _intensity = o._intensity;
    _visible = o._visible;
    markModified(false);
    return *this;
}

Light::~Light()
{
    if (_handle)
        ospRelease(_handle);
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

void Light::commitImpl()
{
    if (!_handle)
    {
        const auto handleName = getOSPHandleName();
        _handle = ospNewLight(handleName.data());
    }

    ospSetParam(_handle, "color", OSPDataType::OSP_VEC3F, &_color);
    ospSetParam(_handle, "intensity", OSPDataType::OSP_FLOAT, &_intensity);
    ospSetParam(_handle, "visible", OSPDataType::OSP_BOOL, &_visible);

    commitLightSpecificParams();

    ospCommit(_handle);
}

OSPLight Light::handle() const noexcept
{
    return _handle;
}
} // namespace brayns
