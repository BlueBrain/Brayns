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

#include "Parser.h"

#include <limits>
#include <string_view>

#include <brayns/core/utils/Binary.h>

#include "Errors.h"

namespace brayns
{
JsonRpcRequest parseJsonRpcRequest(const std::string &text)
{
    auto json = JsonValue();

    try
    {
        json = parseJson(text);
    }
    catch (const JsonException &e)
    {
        throw ParseError(e.what());
    }

    const auto &schema = getJsonSchema<JsonRpcRequest>();

    auto errors = validate(json, schema);

    if (!errors.empty())
    {
        throw InvalidRequest("Invalid request schema", errors);
    }

    return deserializeJsonAs<JsonRpcRequest>(json);
}

JsonRpcRequest parseJsonRpcRequest(const std::vector<char> &binary)
{
    auto data = std::span<const char>(binary);

    if (data.size() < 4)
    {
        throw ParseError("Invalid binary request: expected at least 4 bytes header.");
    }

    auto textSize = extractBytesAs<std::uint32_t>(data, std::endian::little);

    if (data.size() < textSize)
    {
        throw ParseError("Invalid binary request: text size is bigger than total size");
    }

    auto text = extractBytes(data, textSize);

    auto request = parseJsonRpcRequest(std::string(text.data(), text.size()));

    request.params.binary = std::vector<char>(data.begin(), data.end());

    return request;
}

std::string composeAsText(const JsonRpcSuccessResponse &response)
{
    return stringifyToJson(response);
}

std::string composeAsText(const JsonRpcErrorResponse &response)
{
    return stringifyToJson(response);
}

std::vector<char> composeAsBinary(const JsonRpcSuccessResponse &response)
{
    auto text = composeAsText(response);

    auto textSize = text.size();

    if (textSize > std::numeric_limits<std::uint32_t>::max())
    {
        throw InternalError("Text size does not fit in 4 bytes");
    }

    const auto &binary = response.result.binary;

    auto result = std::vector<char>();
    result.reserve(sizeof(std::uint32_t) + text.size() + binary.size());

    composeBytesTo(static_cast<std::uint32_t>(textSize), std::endian::little, result);

    result.insert(result.end(), text.begin(), text.end());
    result.insert(result.end(), binary.begin(), binary.end());

    return result;
}

JsonRpcError composeError(const JsonRpcException &e)
{
    return {
        .code = e.getCode(),
        .message = e.what(),
        .data = e.getData(),
    };
}

JsonRpcErrorResponse composeErrorResponse(const JsonRpcException &e, std::optional<JsonRpcId> id)
{
    return {composeError(e), std::move(id)};
}
}
