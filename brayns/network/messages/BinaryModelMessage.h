/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include <brayns/json/JsonObjectMacro.h>

namespace brayns
{
BRAYNS_JSON_OBJECT_BEGIN(BinaryModelMessage)
BRAYNS_JSON_OBJECT_ENTRY(std::string, chunks_id, "Chunk ID", Required())
BRAYNS_JSON_OBJECT_ENTRY(uint64_t, size, "File syze in bytes", Required())
BRAYNS_JSON_OBJECT_ENTRY(std::string, type, "File extension or type (MESH, POINTS, CIRCUIT)", Required())
BRAYNS_JSON_OBJECT_ENTRY(std::string, loader_name, "Name of the loader to use")
BRAYNS_JSON_OBJECT_ENTRY(std::string, name, "Name of the model in the scene")
BRAYNS_JSON_OBJECT_ENTRY(JsonValue, loader_properties, "Parameters to tune the loading model loading")
BRAYNS_JSON_OBJECT_END()
} // namespace brayns
