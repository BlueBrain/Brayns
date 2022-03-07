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
class DirectionalLight final : public Light
{
public:
    /**
     * @brief Sets the direction this light is facing. The vector will be normalized before storing it.
     */
    void setDirection(const Vector3f& newDirection);

    /**
     * @brief Returns the normalized direction that the light is facing
     */
    const Vector3f& getDirection() const noexcept;

    /**
     * @brief getName() implementation
     */
    std::string_view getName() const noexcept final;

protected:
    std::string_view getOSPHandleName() const noexcept final;

    uint64_t getSizeInBytes() const noexcept final;

    void commitLightSpecificParams() final;

private:
    Vector3f _direction {0.f, 1.f, 0.f};
};

template<>
std::string_view EngineObjectName<DirectionalLight>::get();
}
