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

#include <brayns/engine/Light.h>

namespace brayns
{
class QuadLight final : public Light
{
public:
    QuadLight();

    /**
     * @brief Sets the bottom left corner position of the light (in world space coordinates)
     */
    void setBottomLeftCorner(const Vector3f &pos) noexcept;

    /**
     * @brief Sets a displacement vector to compute the top left corner, computed as
     * top left corner = bottom left corner + vertical displacement.
     */
    void setVerticalDisplacement(const Vector3f &verticalVector) noexcept;

    /**
     * @brief Sets a displacement vector to compute the bottom right corner, computed as
     * bottom right corner = bottom left corner + horizontal displacement.
     */
    void setHorizontalDisplacement(const Vector3f &horizontalVector) noexcept;

    /**
     * @brief Returns the bottom left corner position of the light
     */
    const Vector3f &getBottomLeftCorner() const noexcept;

    /**
     * @brief Returns the vertical displacement vector of the light (used to compute the top left corner from
     * the bottom left corner)
     */
    const Vector3f &getVerticalDisplacement() const noexcept;

    /**
     * @brief Returns the horizontal displacement vector of the light (used to compute the bottom right corner
     * from the bottom left corner)
     */
    const Vector3f &getHorizontalDisplacement() const noexcept;

    Bounds computeBounds() const noexcept override;

protected:
    void commitLightSpecificParams() final;

private:
    // Default crates a X-Plane parallel light of size 1 x 1
    Vector3f _bottomLeftCorner{0.f};
    Vector3f _verticalDisplacement{0.f, 1.f, 0.f};
    Vector3f _horizontalDisplacement{1.f, 0.f, 1.f};
};
}
