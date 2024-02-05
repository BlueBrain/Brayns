/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include <brayns/json/JsonAdapterMacro.h>

enum class PillType
{
    Pill,
    ConePill,
    SigmoidPill
};

namespace brayns
{
BRAYNS_JSON_ADAPTER_ENUM(PillType, {"pill", PillType::Pill},
                         {"conepill", PillType::ConePill},
                         {"sigmoidpill", PillType::SigmoidPill})
} // namespace brayns

BRAYNS_JSON_OBJECT_BEGIN(AddPillMessage)
BRAYNS_JSON_OBJECT_ENTRY(std::string, name, "Name to give to the added model",
                         brayns::Required(false))
BRAYNS_JSON_OBJECT_ENTRY(PillType, type, "Type of pill")
BRAYNS_JSON_OBJECT_ENTRY(brayns::Vector3d, p1,
                         "Center of the lower pill circumference")
BRAYNS_JSON_OBJECT_ENTRY(brayns::Vector3d, p2,
                         "Center of the upper pill circumference")
BRAYNS_JSON_OBJECT_ENTRY(double, radius1,
                         "Radius of the lower pill circumference",
                         brayns::Minimum(0.0))
BRAYNS_JSON_OBJECT_ENTRY(double, radius2,
                         "Radius of the upper pill circumference",
                         brayns::Minimum(0.0))
BRAYNS_JSON_OBJECT_ENTRY(brayns::Vector4d, color, "Pill color RGBA normalized")
BRAYNS_JSON_OBJECT_END()
