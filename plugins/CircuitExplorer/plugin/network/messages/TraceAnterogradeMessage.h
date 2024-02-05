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

#include <brayns/json/JsonObjectMacro.h>

BRAYNS_JSON_OBJECT_BEGIN(TraceAnterogradeMessage)
BRAYNS_JSON_OBJECT_ENTRY(size_t, model_id,
                         "Model where to perform the neuronal tracing")
BRAYNS_JSON_OBJECT_ENTRY(std::vector<uint32_t>, cell_gids,
                         "List of cell GIDs to use a source of the tracing")
BRAYNS_JSON_OBJECT_ENTRY(
    std::vector<uint32_t>, target_cell_gids,
    "List of cells GIDs which are the result of the given tracing mode")
BRAYNS_JSON_OBJECT_ENTRY(
    brayns::Vector4d, source_cell_color,
    "RGBA normalized color to apply to the source cell geometry")
BRAYNS_JSON_OBJECT_ENTRY(
    brayns::Vector4d, connected_cells_color,
    "RGBA normalized color to apply to the target cells geometry")
BRAYNS_JSON_OBJECT_ENTRY(brayns::Vector4d, non_connected_cells_color,
                         "RGBA normalized color to apply to the rest of cells")
BRAYNS_JSON_OBJECT_END()