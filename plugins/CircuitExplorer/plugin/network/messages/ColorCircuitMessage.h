/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: nadir.romanguerrero@epfl.ch
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

#include <plugin/network/adapters/ColoringInfoAdapter.h>

BRAYNS_JSON_OBJECT_BEGIN(ColorCircuitByIdMessage)
BRAYNS_JSON_OBJECT_ENTRY(size_t, model_id, "ID of the model to color")
BRAYNS_JSON_OBJECT_ENTRY(
    std::vector<ColoringInformation>, color_info,
    "List of IDs with their corresponding color. If empty, all the "
    "model will be colored with random colors per ID")
BRAYNS_JSON_OBJECT_END()

BRAYNS_JSON_OBJECT_BEGIN(ColorCircuitBySingleColorMessage)
BRAYNS_JSON_OBJECT_ENTRY(size_t, model_id, "ID of the model to color")
BRAYNS_JSON_OBJECT_ENTRY(brayns::Vector4f, color,
                         "Color to use for the whole circuit "
                         "(Normalized RGBA)")
BRAYNS_JSON_OBJECT_END()

BRAYNS_JSON_OBJECT_BEGIN(ColorCircuitByMethodMessage)
BRAYNS_JSON_OBJECT_ENTRY(size_t, model_id, "ID of the model to color")
BRAYNS_JSON_OBJECT_ENTRY(std::string, method, "Method to use for coloring")
BRAYNS_JSON_OBJECT_ENTRY(
    std::vector<ColoringInformation>, color_info,
    "List of IDs with their corresponding color. If empty, all the "
    "model will be colored with random colors per ID")
BRAYNS_JSON_OBJECT_END()
