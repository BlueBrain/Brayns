/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <brayns/common/Bounds.h>

namespace brayns
{
Bounds::Bounds(const Vector3f &minB, const Vector3f &maxB)
    : _min(minB)
    , _max(maxB)
{
    if (glm::min(_min, _max) != _min)
    {
        throw std::invalid_argument("The min bounds must be smaller or equal to the max bounds");
    }
}

void Bounds::expand(const Vector3f &point) noexcept
{
    _min = glm::min(_min, point);
    _max = glm::max(_max, point);
}

void Bounds::expand(const Bounds &bounds) noexcept
{
    _min = glm::min(_min, bounds._min);
    _max = glm::max(_max, bounds._max);
}

bool Bounds::intersects(const Vector3f &point) const noexcept
{
    return point.x >= _min.x && point.x <= _max.x && point.y >= _min.y && point.y <= _max.y && point.z >= _min.z
        && point.z <= _max.z;
}

bool Bounds::intersects(const Bounds &other) const noexcept
{
    // slab comparsion

    const auto xStart = _min.x;
    const auto xEnd = _max.x;
    const auto otherXStart = other._min.x;
    const auto otherXEnd = other._max.x;

    if (xStart > otherXEnd || xEnd < otherXStart)
    {
        return false;
    }

    const auto yStart = _min.y;
    const auto yEnd = _max.y;
    const auto otherYStart = other._min.y;
    const auto otherYEnd = other._max.y;

    if (yStart > otherYEnd || yEnd < otherYStart)
    {
        return false;
    }

    const auto zStart = _min.z;
    const auto zEnd = _max.z;
    const auto otherZStart = other._min.z;
    const auto otherZEnd = other._max.z;

    if (zStart > otherZEnd || zEnd < otherZStart)
    {
        return false;
    }

    return true;
}

const Vector3f &Bounds::getMin() const noexcept
{
    return _min;
}

const Vector3f &Bounds::getMax() const noexcept
{
    return _max;
}

Vector3f Bounds::center() const noexcept
{
    return (_max + _min) * 0.5f;
}

Vector3f Bounds::dimensions() const noexcept
{
    return _max - _min;
}
}
