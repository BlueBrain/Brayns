/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <spdlog/fmt/fmt.h>

#include <brayns/core/json/Json.h>

#include <brayns/core/utils/parsing/Parser.h>
#include <brayns/core/utils/string/StringExtractor.h>

#include "JsonRpcException.h"

namespace
{
class JsonParser
{
public:
    static brayns::JsonValue parse(std::string_view data)
    {
        try
        {
            return brayns::Json::parse(std::string(data));
        }
        catch (const brayns::JsonParsingError &e)
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
        auto errors = brayns::Json::validate(json, _schema);
        if (!errors.empty())
        {
            throw brayns::InvalidRequestException("Invalid JSON-RPC request schema", errors);
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
            throw brayns::InvalidRequestException(fmt::format("Unsupported JSON-RPC version: '{}'", message.jsonrpc));
        }
        auto &method = message.method;
        if (method.empty())
        {
            throw brayns::InvalidRequestException("No methods provided in request");
        }
    }
};

class RequestMessageParser
{
public:
    static brayns::RequestMessage parse(std::string_view data)
    {
        auto json = JsonParser::parse(data);
        RequestSchemaValidator::validate(json);
        auto message = brayns::Json::deserialize<brayns::RequestMessage>(json);
        RequestHeaderValidator::validate(message);
        return message;
    }
};

class JsonSizeParser
{
public:
    static size_t extract(std::string_view &data)
    {
        auto size = data.size();
        if (size < 4)
        {
            throw brayns::ParsingErrorException("Invalid binary packet, expected at least 4 bytes for the JSON size.");
        }
        auto jsonSize = brayns::Parser::extractChunk<uint32_t>(data, std::endian::little);
        if (jsonSize > size - 4)
        {
            throw brayns::ParsingErrorException(fmt::format("Invalid binary packet, JSON size too bi: {}", jsonSize));
        }
        return jsonSize;
    }
};

class BinaryParser
{
public:
    static brayns::JsonRpcRequest parse(const brayns::ClientRequest &request)
    {
        auto &client = request.getClient();
        auto data = request.getData();
        auto size = JsonSizeParser::extract(data);
        auto json = brayns::StringExtractor::extract(data, size);
        auto message = RequestMessageParser::parse(json);
        auto binary = std::string(data);
        return {client, std::move(message), std::move(binary)};
    }
};

class TextParser
{
public:
    static brayns::JsonRpcRequest parse(const brayns::ClientRequest &request)
    {
        auto &client = request.getClient();
        auto data = request.getData();
        auto message = RequestMessageParser::parse(data);
        return {client, std::move(message)};
    }
};
} // namespace

namespace brayns
{
JsonRpcRequest JsonRpcParser::parse(const ClientRequest &request)
{
    if (request.isBinary())
    {
        return BinaryParser::parse(request);
    }
    if (request.isText())
    {
        return TextParser::parse(request);
    }
    throw InvalidRequestException("Expected text or binary request");
}
} // namespace brayns
