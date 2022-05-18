/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include <brayns/io/LoadParameters.h>

namespace brayns
{
BRAYNS_JSON_ADAPTER_BEGIN(BinaryLoadParameters)
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("chunks_id", chunksID, "Chunk ID");
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("size", size, "File size in bytes");
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("type", type, "File extension");
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("loader_name", loaderName, "Loader name");
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("loader_properties", loadParameters, "Loader properties");
BRAYNS_JSON_ADAPTER_END()
} // namespace brayns
