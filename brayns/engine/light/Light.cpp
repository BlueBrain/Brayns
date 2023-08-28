/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

namespace brayns
{
Light::Light(Light &&other) noexcept
{
    *this = std::move(other);
}

Light &Light::operator=(Light &&other) noexcept
{
    _handleName = std::move(other._handleName);
    _lightName = std::move(other._lightName);
    _handle = std::move(other._handle);
    _data = std::move(other._data);
    return *this;
}

Light::Light(const Light &other)
{
    *this = other;
}

Light &Light::operator=(const Light &other)
{
    _handleName = other._handleName;
    _lightName = other._lightName;
    _handle = ospray::cpp::Light(_handleName);
    _data = other._data->clone();
    _data->pushTo(_handle);
    _handle.commit();
    return *this;
}

const ospray::cpp::Light &Light::getHandle() const noexcept
{
    return _handle;
}

Bounds Light::computeBounds(const TransformMatrix &matrix) const noexcept
{
    return _data->computeBounds(matrix);
}
}
