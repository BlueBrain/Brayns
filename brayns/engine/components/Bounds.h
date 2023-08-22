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

#pragma once

#include <brayns/utils/MathTypes.h>

namespace brayns
{
class Bounds
{
public:
    /**
     * @brief Default initializes min to {max float} and max to {lowest float} in anticipation of the use of expand().
     */
    Bounds() = default;

    /**
     * @brief Initializes bounds with pre-defined boundaries.
     * @param minB
     * @param maxB
     */
    Bounds(const Vector3f &minB, const Vector3f &maxB);

    /**
     * @brief Expands the bounds (if needed) to include the given point.
     * @param point
     */
    void expand(const Vector3f &point) noexcept;

    /**
     * @brief Expands the bounds (if needed) to include the given bounds.
     * @param bounds
     */
    void expand(const Bounds &bounds) noexcept;

    /**
     * @brief Returns true if the given point is within the bounds.
     * @param point
     * @return bool
     */
    bool intersects(const Vector3f &point) const noexcept;

    /**
     * @brief Returns true if the given bounds collide with this bounds.
     * @param bounds
     * @return bool
     */
    bool intersects(const Bounds &bounds) const noexcept;

    /**
     * @brief Returns the minimum corner of the bounds.
     * @return const Vector3f &
     */
    const Vector3f &getMin() const noexcept;

    /**
     * @brief Returns the maximum corner of the bounds.
     * @return const Vector3f &
     */
    const Vector3f &getMax() const noexcept;

    /**
     * @brief Returns the bounds center
     *
     * @return Vector3f
     */
    Vector3f center() const noexcept;

    /**
     * @brief Return the length on each axis
     *
     * @return Vector3f
     */
    Vector3f dimensions() const noexcept;

private:
    math::box3f _bounds;
};
}
