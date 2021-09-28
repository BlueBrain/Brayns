/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#include <brayns/network/json/Message.h>

namespace brayns
{
BRAYNS_MESSAGE_BEGIN(SchemaParams)
BRAYNS_MESSAGE_ENTRY(std::string, endpoint, "Name of the endpoint")
BRAYNS_MESSAGE_END()

BRAYNS_MESSAGE_BEGIN(SchemaResult)
BRAYNS_MESSAGE_ENTRY(std::string, plugin,
                     "Name of the plugin that loads the entrypoint")
BRAYNS_MESSAGE_ENTRY(std::string, title, "Name of the entrypoint")
BRAYNS_MESSAGE_ENTRY(std::string, description, "Description of the entrypoint")
BRAYNS_MESSAGE_ENTRY(std::string, type, "Type of entrypoint ('method')")
BRAYNS_MESSAGE_ENTRY(bool, async, "Check if the entrypoint is asynchronous")
BRAYNS_MESSAGE_ENTRY(std::vector<JsonSchema>, params, "Input schema")
BRAYNS_MESSAGE_ENTRY(JsonSchema, returns, "Output schema")
BRAYNS_MESSAGE_END()
} // namespace brayns