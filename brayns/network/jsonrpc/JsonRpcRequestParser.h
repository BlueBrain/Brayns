/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <string>

#include "JsonRpcRequest.h"

namespace brayns
{
/**
 * @brief Helper class to parse and validate a JSON-RPC request.
 *
 */
class JsonRpcRequestParser
{
public:
    /**
     * @brief Parse data to create a JSON-RPC request message.
     *
     * @param data Raw JSON data.
     * @return brayns::RequestMessage Parsed request message.
     * @throw JsonRpcException If any errors occur that must be reported.
     */
    static brayns::RequestMessage parse(const std::string &data);
};
} // namespace brayns
