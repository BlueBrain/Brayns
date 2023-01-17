/* Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

#include <brayns/json/JsonObjectMacro.h>

namespace brayns
{
BRAYNS_JSON_OBJECT_BEGIN(InspectMessage)
BRAYNS_JSON_OBJECT_ENTRY(Vector2f, position, "Position XY (normalized)")
BRAYNS_JSON_OBJECT_END()

BRAYNS_JSON_OBJECT_BEGIN(InspectResult)
BRAYNS_JSON_OBJECT_ENTRY(
    bool,
    hit,
    "A boolean flag indicating wether there was a hit. If false, the rest of the fields must be ignored")
BRAYNS_JSON_OBJECT_ENTRY(Vector3f, position, "3D hit position")
BRAYNS_JSON_OBJECT_ENTRY(uint32_t, model_id, "ID of the model hitted")
BRAYNS_JSON_OBJECT_ENTRY(JsonValue, metadata, "Extra attributes which vary depending on the type of model hitted")
BRAYNS_JSON_OBJECT_END()
} // namespace brayns
