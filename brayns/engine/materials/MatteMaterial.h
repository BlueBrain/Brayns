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
#include <brayns/json/JsonAdapterMacro.h>

namespace brayns
{
class MatteMaterial : public Material
{
public:
    MatteMaterial();

    void commitMaterialSpecificParams() final;

    std::string_view getName() const noexcept final;

    void setColor(const Vector3f& color) noexcept;
    void setOpacity(const float opacity) noexcept;

    const Vector3f& getColor() const noexcept;
    float getOpacity() const noexcept;

private:
    Vector3f _color {1.f}; // default white
    float _opacity {1.f};  // default opaque
};

BRAYNS_JSON_ADAPTER_BEGIN(MatteMaterial)
BRAYNS_JSON_ADAPTER_GETSET("color", getColor, setColor, "Base color of the material")
BRAYNS_JSON_ADAPTER_GETSET("opacity", getOpacity, setOpacity,
                           "Base opacity of the material. Will be clampled to the range [0.0, 1.0]")
BRAYNS_JSON_ADAPTER_END()
}
