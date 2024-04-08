/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "Geometry.h"

namespace brayns
{
Geometry::Geometry(Geometry &&other) noexcept
{
    *this = std::move(other);
}

Geometry &Geometry::operator=(Geometry &&other) noexcept
{
    _handleName = std::move(other._handleName);
    _geometryName = std::move(other._geometryName);
    _handle = std::move(other._handle);
    _data = std::move(other._data);
    _flag = std::move(other._flag);
    return *this;
}

Geometry::Geometry(const Geometry &other)
{
    *this = other;
}

Geometry &Geometry::operator=(const Geometry &other)
{
    _handleName = other._handleName;
    _geometryName = other._geometryName;
    _handle = ospray::cpp::Geometry(_handleName);
    _data = other._data->clone();
    _flag.setModified(true);
    return *this;
}

size_t Geometry::numPrimitives() const noexcept
{
    return _data->numPrimitives();
}

Bounds Geometry::computeBounds(const TransformMatrix &matrix) const noexcept
{
    return _data->computeBounds(matrix);
}

bool Geometry::commit()
{
    if (!_flag)
    {
        return false;
    }
    _data->pushTo(_handle);
    _flag = false;
    _handle.commit();
    return true;
}

const ospray::cpp::Geometry &Geometry::getHandle() const noexcept
{
    return _handle;
}
}
