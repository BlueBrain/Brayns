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

#pragma once

#include <brayns/common/MathTypes.h>

namespace brayns
{
class Bounds
{
public:
    Bounds() = default;
    Bounds(const Vector3f& minB, const Vector3f& maxB);

    void expand(const Vector3f& point) noexcept;
    void expand(const Bounds& bounds) noexcept;

    bool intersects(const Vector3f& point) const noexcept;
    bool intersects(const Bounds& bounds) const noexcept;

    const Vector3f& min() const noexcept;
    const Vector3f& max() const noexcept;
private:
    Vector3f _min {std::numeric_limits<float>::max()};
    Vector3f _max {std::numeric_limits<float>::lowest()};
};
}
