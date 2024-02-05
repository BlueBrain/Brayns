/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/json/JsonObjectMacro.h>

#include "RequestId.h"

namespace brayns
{
/**
 * @brief Message used to send a request to brayns.
 *
 */
BRAYNS_JSON_OBJECT_BEGIN(RequestMessage)
BRAYNS_JSON_OBJECT_ENTRY(std::string, jsonrpc, "Protocol version")
BRAYNS_JSON_OBJECT_ENTRY(RequestId, id, "Message ID", Required(false))
BRAYNS_JSON_OBJECT_ENTRY(std::string, method, "Entrypoint name")
BRAYNS_JSON_OBJECT_ENTRY(JsonValue, params, "Request content", Required(false))
BRAYNS_JSON_OBJECT_END()

/**
 * @brief Cancel info received in cancel message params.
 *
 */
BRAYNS_JSON_OBJECT_BEGIN(CancelParams)
BRAYNS_JSON_OBJECT_ENTRY(RequestId, id, "ID of the request to cancel")
BRAYNS_JSON_OBJECT_END()

/**
 * @brief Message used by brayns to send the reply.
 *
 */
BRAYNS_JSON_OBJECT_BEGIN(ReplyMessage)
BRAYNS_JSON_OBJECT_ENTRY(std::string, jsonrpc, "Protocol version")
BRAYNS_JSON_OBJECT_ENTRY(RequestId, id, "Message ID")
BRAYNS_JSON_OBJECT_ENTRY(std::string, method, "Entrypoint name")
BRAYNS_JSON_OBJECT_ENTRY(JsonValue, result, "Reply content")
BRAYNS_JSON_OBJECT_END()

/**
 * @brief Description of an error used in error message.
 *
 */
BRAYNS_JSON_OBJECT_BEGIN(ErrorDescriptionMessage)
BRAYNS_JSON_OBJECT_ENTRY(int, code, "Error code")
BRAYNS_JSON_OBJECT_ENTRY(std::string, message, "Error description")
BRAYNS_JSON_OBJECT_ENTRY(JsonValue, data, "Additional error info")
BRAYNS_JSON_OBJECT_END()

/**
 * @brief Message used by brayns to send an error reply.
 *
 */
BRAYNS_JSON_OBJECT_BEGIN(ErrorMessage)
BRAYNS_JSON_OBJECT_ENTRY(std::string, jsonrpc, "Protocol version")
BRAYNS_JSON_OBJECT_ENTRY(RequestId, id, "Message ID")
BRAYNS_JSON_OBJECT_ENTRY(std::string, method, "Entrypoint name")
BRAYNS_JSON_OBJECT_ENTRY(ErrorDescriptionMessage, error, "Error object")
BRAYNS_JSON_OBJECT_END()

/**
 * @brief Progress info used in progress messages.
 *
 */
BRAYNS_JSON_OBJECT_BEGIN(ProgressInfoMessage)
BRAYNS_JSON_OBJECT_ENTRY(RequestId, id, "Request ID")
BRAYNS_JSON_OBJECT_ENTRY(std::string, operation, "Operation description")
BRAYNS_JSON_OBJECT_ENTRY(double, amount, "Progress percentage [0-1]")
BRAYNS_JSON_OBJECT_END()

/**
 * @brief Progress message sent during a request processing.
 *
 */
BRAYNS_JSON_OBJECT_BEGIN(ProgressMessage)
BRAYNS_JSON_OBJECT_ENTRY(std::string, jsonrpc, "Protocol version")
BRAYNS_JSON_OBJECT_ENTRY(std::string, method, "Entrypoint name")
BRAYNS_JSON_OBJECT_ENTRY(ProgressInfoMessage, params, "Progression info")
BRAYNS_JSON_OBJECT_END()

/**
 * @brief Message send spontaneously by brayns.
 *
 */
BRAYNS_JSON_OBJECT_BEGIN(NotificationMessage)
BRAYNS_JSON_OBJECT_ENTRY(std::string, jsonrpc, "Protocol version")
BRAYNS_JSON_OBJECT_ENTRY(std::string, method, "Entrypoint name")
BRAYNS_JSON_OBJECT_ENTRY(JsonValue, params, "Message content")
BRAYNS_JSON_OBJECT_END()

/**
 * @brief Empty message when no params or result is needed.
 *
 */
BRAYNS_JSON_OBJECT_BEGIN(EmptyMessage)
BRAYNS_JSON_OBJECT_END()

/**
 * @brief Serialize empty message as empty object.
 *
 */
template <>
struct JsonAdapter<EmptyMessage>
{
    static JsonSchema getSchema(const EmptyMessage&) { return {}; }

    static bool serialize(const EmptyMessage&, JsonValue& json)
    {
        json = Poco::makeShared<JsonObject>();
        return true;
    }

    static bool deserialize(const JsonValue&, EmptyMessage&) { return true; }
};

/**
 * @brief Helper class to create standard messages.
 *
 */
class MessageFactory
{
public:
    /**
     * @brief Create a ReplyMessage corresponding to a RequestMessage.
     *
     * The resulting reply message will have the same attributes as the request
     * (id, method, etc) and empty params (to be filled with message content).
     *
     * @param request Request message containing the transaction info.
     * @return ReplyMessage The reply message corresponding to request.
     */
    static ReplyMessage createReply(const RequestMessage& request)
    {
        ReplyMessage reply;
        reply.jsonrpc = request.jsonrpc;
        reply.id = request.id;
        reply.method = request.method;
        return reply;
    }

    /**
     * @brief Create an ErrorMessage corresponding to a RequestMessage.
     *
     * The resulting error message will have the same attributes as the request
     * (id, method, etc) and an empty description (to be filled with error
     * info).
     *
     * @param request Request message containing the transaction info.
     * @return ErrorMessage The error message corresponding to request.
     */
    static ErrorMessage createError(const RequestMessage& request)
    {
        ErrorMessage error;
        error.jsonrpc = request.jsonrpc;
        error.id = request.id;
        error.method = request.method;
        return error;
    }

    /**
     * @brief Create a ProgressMessage corresponding to a RequestMessage.
     *
     * The resulting progress message will have the same id as the request and
     * empty description and amount (to be filled with message content).
     *
     * @param request Request message containing the transaction info.
     * @return ProgressMessage The progress message corresponding to request.
     */
    static ProgressMessage createProgress(const RequestMessage& request)
    {
        ProgressMessage progress;
        progress.jsonrpc = request.jsonrpc;
        progress.method = "progress";
        progress.params.id = request.id;
        return progress;
    }
};
} // namespace brayns
