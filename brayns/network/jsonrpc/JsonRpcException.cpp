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

#include "JsonRpcException.h"

#include <spdlog/fmt/fmt.h>

namespace
{
class ErrorFormatter
{
public:
    static brayns::JsonValue format(const brayns::JsonErrors &errors)
    {
        auto messages = brayns::JsonErrorFormatter::format(errors);
        return brayns::Json::serialize(messages);
    }
};
} // namespace

namespace brayns
{
JsonRpcException::JsonRpcException(const std::string &message):
    std::runtime_error(message)
{
}

JsonRpcException::JsonRpcException(int code, const std::string &message, const JsonValue &data):
    std::runtime_error(message),
    _code(code),
    _data(data)
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

ParsingErrorException::ParsingErrorException(const std::string &message):
    JsonRpcException(-32700, fmt::format("Invalid JSON: '{}'", message))
{
}

InvalidRequestException::InvalidRequestException(const std::string &message):
    JsonRpcException(-32600, message)
{
}

InvalidRequestException::InvalidRequestException(const std::string &message, const JsonErrors &errors):
    JsonRpcException(-32600, message, ErrorFormatter::format(errors))
{
}

MethodNotFoundException::MethodNotFoundException(const std::string &method):
    JsonRpcException(-32601, fmt::format("Method '{}' not found", method))
{
}

InvalidParamsException::InvalidParamsException(const std::string &message):
    JsonRpcException(-32602, message)
{
}

InvalidParamsException::InvalidParamsException(const std::string &message, const JsonErrors &errors):
    JsonRpcException(-32602, message, ErrorFormatter::format(errors))
{
}

InternalErrorException::InternalErrorException(const std::string &message):
    JsonRpcException(-32603, message)
{
}

TaskCancelledException::TaskCancelledException():
    JsonRpcException(20, "Task has been cancelled")
{
}
} // namespace brayns
