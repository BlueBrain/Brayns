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

#include "JsonRpcParser.h"

#include <Poco/JSON/JSONException.h>

#include <brayns/json/Json.h>
#include <brayns/json/JsonSchemaValidator.h>

#include "JsonRpcException.h"

namespace
{
class JsonParser
{
public:
    static brayns::JsonValue parse(const std::string &data)
    {
        try
        {
            return brayns::Json::parse(data);
        }
        catch (const Poco::JSON::JSONException &e)
        {
            throw brayns::ParsingErrorException(e.displayText());
        }
    }
};

class RequestSchemaValidator
{
public:
    static void validate(const brayns::JsonValue &json)
    {
        auto errors = brayns::JsonSchemaValidator::validate(json, _schema);
        if (!errors.empty())
        {
            throw brayns::InvalidRequestException("Invalid request schema", errors);
        }
    }

private:
    static inline const auto _schema = brayns::Json::getSchema<brayns::RequestMessage>();
};

class RequestHeaderValidator
{
public:
    static void validate(const brayns::RequestMessage &message)
    {
        if (message.jsonrpc != "2.0")
        {
            throw brayns::InvalidRequestException("Unsupported JSON-RPC version: '" + message.jsonrpc + "'");
        }
        auto &method = message.method;
        if (method.empty())
        {
            throw brayns::InvalidRequestException("No methods provided in request");
        }
    }
};

class ParserHelper
{
public:
    static brayns::RequestMessage parse(const std::string &data)
    {
        auto json = JsonParser::parse(data);
        RequestSchemaValidator::validate(json);
        auto message = brayns::Json::deserialize<brayns::RequestMessage>(json);
        RequestHeaderValidator::validate(message);
        return message;
    }
};
} // namespace

namespace brayns
{
JsonRpcRequest JsonRpcParser::parse(const ClientRequest &request)
{
    auto &client = request.getClient();
    auto data = std::string(request.getData());
    auto message = ParserHelper::parse(data);
    return {client, std::move(message)};
}
} // namespace brayns
