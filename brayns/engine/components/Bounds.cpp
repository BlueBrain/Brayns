/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include <brayns/engine/components/Bounds.h>

#include <rkcommon/math/box.h>

namespace brayns
{
Bounds::Bounds(const AxisAlignedBounds &box):
    box(box)
{
}

Bounds::Bounds(const Vector3f &minB, const Vector3f &maxB):
    box(minB, maxB)
{
    if (math::min(minB, maxB) != minB)
    {
        throw std::invalid_argument("The min bounds must be smaller or equal to the max bounds");
    }
}

void Bounds::expand(const Vector3f &point) noexcept
{
    box.extend(point);
}

void Bounds::expand(const Bounds &bounds) noexcept
{
    box.extend(bounds.box);
}

bool Bounds::intersects(const Vector3f &point) const noexcept
{
    return box.contains(point);
}

bool Bounds::intersects(const Bounds &other) const noexcept
{
    return math::touchingOrOverlapping(box, other.box);
}

const Vector3f &Bounds::getMin() const noexcept
{
    return box.lower;
}

const Vector3f &Bounds::getMax() const noexcept
{
    return box.upper;
}

Vector3f Bounds::center() const noexcept
{
    return box.center();
}

Vector3f Bounds::dimensions() const noexcept
{
    return box.size();
}
}
