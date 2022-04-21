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
#include <brayns/engine/Material.h>

namespace brayns
{
class MetalMaterial final : public Material
{
public:
    std::string getName() const noexcept final;

    uint64_t getSizeInBytes() const noexcept final;

    /**
     * @brief Sets the surface roughness
     */
    void setRoughness(const float roughness) noexcept;

    /**
     * @brief Returns the surface roughness
     */
    float getRoughness() const noexcept;

protected:
    std::string getOSPHandleName() const noexcept final;

    void commitMaterialSpecificParams() final;

private:
    float _roughness{0.1f};
};
}
