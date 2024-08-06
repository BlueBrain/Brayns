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
JsonRpcRequest parseTextJsonRpcRequest(const std::string &text)
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

    return deserializeAs<JsonRpcRequest>(json);
}

JsonRpcRequest parseBinaryJsonRpcRequest(const std::string &binary)
{
    auto data = std::string_view(binary);

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

    auto request = parseTextJsonRpcRequest(std::string(text));

    request.params.binary = binary.substr(4 + textSize);

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

std::string composeAsBinary(const JsonRpcSuccessResponse &response)
{
    auto text = composeAsText(response);

    auto textSize = text.size();

    if (textSize > std::numeric_limits<std::uint32_t>::max())
    {
        throw InternalError("Text size does not fit in 4 bytes");
    }

    auto header = composeBytes(static_cast<std::uint32_t>(textSize), std::endian::little);

    return header + text + response.result.binary;
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
