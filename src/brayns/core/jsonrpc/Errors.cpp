/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include "Errors.h"

namespace brayns
{
template<>
struct JsonReflector<JsonSchemaError>
{
    static JsonSchema getSchema()
    {
        throw std::invalid_argument("Not implemented");
    }

    static void serialize(const JsonSchemaError &value, JsonValue &json)
    {
        auto object = createJsonObject();

        auto path = toString(value.path);
        object->set("path", path);

        auto error = toString(value.error);
        object->set("error", error);

        json = object;
    }

    static void deserialize(const JsonValue &, JsonSchemaError &)
    {
        throw std::invalid_argument("Not implemented");
    }
};

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

ParseError::ParseError(const std::string &message):
    JsonRpcException(-32'700, message)
{
}

InvalidRequest::InvalidRequest(const std::string &message):
    JsonRpcException(-32'600, message)
{
}

InvalidRequest::InvalidRequest(const std::string &message, const std::vector<JsonSchemaError> &errors):
    JsonRpcException(-32'600, message, serializeToJson(errors))
{
}

MethodNotFound::MethodNotFound(const std::string &method):
    JsonRpcException(-32'601, fmt::format("Method not found: '{}'", method))
{
}

InvalidParams::InvalidParams(const std::string &message):
    JsonRpcException(-32'602, message)
{
}

InvalidParams::InvalidParams(const std::string &message, const std::vector<JsonSchemaError> &errors):
    JsonRpcException(-32'602, message, serializeToJson(errors))
{
}

InternalError::InternalError(const std::string &message):
    JsonRpcException(-32'603, message)
{
}

InternalError::InternalError(const std::exception &e):
    JsonRpcException(-32'603, e.what())
{
}
}
