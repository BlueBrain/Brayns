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

#include "JsonRpcException.h"

#include <brayns/json/Json.h>
#include <brayns/json/adapters/ArrayAdapter.h>
#include <brayns/json/adapters/PrimitiveAdapter.h>

namespace brayns
{
JsonRpcException::JsonRpcException(const std::string &message)
    : std::runtime_error(message)
{
}

JsonRpcException::JsonRpcException(int code, const std::string &message, const JsonValue &data)
    : std::runtime_error(message)
    , _code(code)
    , _data(data)
{
}

int JsonRpcException::getCode() const
{
    return _code;
}

const JsonValue &JsonRpcException::getData() const
{
    return _data;
}

ParsingErrorException::ParsingErrorException(const std::string &message)
    : JsonRpcException(-32700, "Failed to parse JSON request: " + message)
{
}

InvalidRequestException::InvalidRequestException(const std::string &message)
    : JsonRpcException(-32600, message)
{
}

InvalidRequestException::InvalidRequestException(const std::string &message, const std::vector<std::string> &errors)
    : JsonRpcException(-32600, message, Json::serialize(errors))
{
}

MethodNotFoundException::MethodNotFoundException(const std::string &method)
    : JsonRpcException(-32601, "Method not found: '" + method + "'")
{
}

InvalidParamsException::InvalidParamsException(const std::string &message)
    : JsonRpcException(-32602, message)
{
}

InvalidParamsException::InvalidParamsException(const std::string &message, const std::vector<std::string> &errors)
    : JsonRpcException(-32602, message, Json::serialize(errors))
{
}

InternalErrorException::InternalErrorException(const std::string &message)
    : JsonRpcException(-32603, message)
{
}
} // namespace brayns
