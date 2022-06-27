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
     * @param position The new light corner position
     */
    void setPosition(const Vector3f &position) noexcept;

    /**
     * @brief Sets one of the square light edges
     * @param edge1 Edge vector
     */
    void setEdge1(const Vector3f &edge1) noexcept;

    /**
     * @brief Sets one of the square light edges
     * @param edge2 Edge vector
     */
    void setEdge2(const Vector3f &edge2) noexcept;

    /**
     * @brief Returns the light corner position
     */
    const Vector3f &getPosition() const noexcept;

    /**
     * @brief Returns one of the light edges
     */
    const Vector3f &getEdge1() const noexcept;

    /**
     * @brief Returns one of the light edges
     */
    const Vector3f &getEdge2() const noexcept;

    Bounds computeBounds() const noexcept override;

protected:
    void commitLightSpecificParams() final;

private:
    // Default crates a X-Plane parallel light of size 1 x 1
    Vector3f _position{0.f};
    Vector3f _edge1{0.f, 1.f, 0.f};
    Vector3f _edge2{1.f, 0.f, 1.f};
};
}
