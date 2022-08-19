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

namespace brayns
{
Light::Light(const Light &other)
{
    *this = other;
}

Light &Light::operator=(const Light &other)
{
    _osprayHandleName = other._osprayHandleName;
    _lightName = other._lightName;
    _handle = ospray::cpp::Light(_osprayHandleName);
    _data = other._data->clone();
    _data->pushTo(_handle);
    _handle.commit();
}

const ospray::cpp::Light &Light::getHandle() const noexcept
{
    return _handle;
}

Bounds Light::computeBounds(const Matrix4f &matrix) const noexcept
{
    return _data->computeBounds(matrix);
}
}
