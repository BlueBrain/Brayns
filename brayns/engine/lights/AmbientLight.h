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
#include <brayns/json/JsonAdapterMacro.h>

namespace brayns
{
class AmbientLight : public Light
{
public:
    /**
     * @brief getName() implementation
     */
    std::string_view getName() const noexcept final;

protected:
    std::string_view getOSPHandleName() const noexcept final;

    uint64_t getSizeInBytes() const noexcept final;

    void commitLightSpecificParams() final;
};

BRAYNS_JSON_ADAPTER_BEGIN(AmbientLight)
BRAYNS_JSON_ADAPTER_GETSET("color", getColor, setColor, "Light color (Normalized RGB)")
BRAYNS_JSON_ADAPTER_GETSET("intensity", getIntensity, setIntensity,
                           "Light intensity (Will be clamped on the range [0.0, +infinity)")
BRAYNS_JSON_ADAPTER_GETSET("visible", isVisible, setVisible, "Sets wether the light should be visible on the scene")
BRAYNS_JSON_ADAPTER_END()
}
