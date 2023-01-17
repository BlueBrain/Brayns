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

#include <brayns/engine/json/adapters/MaterialAdapters.h>
#include <brayns/json/JsonBuffer.h>

namespace brayns
{
template<typename MaterialType>
struct SetMaterialMessage
{
    uint32_t model_id = 0;
    JsonBuffer<MaterialType> material;
};

#define BRAYNS_SET_MATERIAL_ADAPTER(TYPE) \
    BRAYNS_JSON_ADAPTER_BEGIN(SetMaterialMessage<TYPE>) \
    BRAYNS_JSON_ADAPTER_ENTRY(model_id, "Model ID") \
    BRAYNS_JSON_ADAPTER_ENTRY(material, "Material parameters") \
    BRAYNS_JSON_ADAPTER_END()

BRAYNS_SET_MATERIAL_ADAPTER(CarPaint)
BRAYNS_SET_MATERIAL_ADAPTER(Emissive)
BRAYNS_SET_MATERIAL_ADAPTER(Glass)
BRAYNS_SET_MATERIAL_ADAPTER(Matte)
BRAYNS_SET_MATERIAL_ADAPTER(Metal)
BRAYNS_SET_MATERIAL_ADAPTER(Phong)
BRAYNS_SET_MATERIAL_ADAPTER(Plastic)
}
