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

#include <string>

#include "Message.h"

namespace brayns
{
/**
 * @brief Message used to send a request to brayns.
 *
 */
BRAYNS_MESSAGE_BEGIN(RequestMessage)
BRAYNS_MESSAGE_ENTRY(std::string, jsonrpc, "Protocol version")
BRAYNS_MESSAGE_OPTION(std::string, id, "Message ID")
BRAYNS_MESSAGE_ENTRY(std::string, method, "Entrypoint name")
BRAYNS_MESSAGE_ENTRY(JsonValue, params, "Request content")
BRAYNS_MESSAGE_END()

/**
 * @brief Cancel info received in cancel message params.
 *
 */
BRAYNS_MESSAGE_BEGIN(CancelParams)
BRAYNS_MESSAGE_ENTRY(std::string, id, "Request ID")
BRAYNS_MESSAGE_END()

/**
 * @brief Message used by brayns to send the reply.
 *
 */
BRAYNS_MESSAGE_BEGIN(ReplyMessage)
BRAYNS_MESSAGE_ENTRY(std::string, jsonrpc, "Protocol version")
BRAYNS_MESSAGE_ENTRY(std::string, id, "Message ID")
BRAYNS_MESSAGE_ENTRY(std::string, method, "Entrypoint name")
BRAYNS_MESSAGE_ENTRY(JsonValue, result, "Reply content")
BRAYNS_MESSAGE_END()

/**
 * @brief Description of an error used in error message.
 *
 */
BRAYNS_MESSAGE_BEGIN(ErrorDescriptionMessage)
BRAYNS_MESSAGE_ENTRY(int, code, "Error code")
BRAYNS_MESSAGE_ENTRY(std::string, message, "Error description")
BRAYNS_MESSAGE_END()

/**
 * @brief Message used by brayns to send an error reply.
 *
 */
BRAYNS_MESSAGE_BEGIN(ErrorMessage)
BRAYNS_MESSAGE_ENTRY(std::string, jsonrpc, "Protocol version")
BRAYNS_MESSAGE_ENTRY(std::string, id, "Message ID")
BRAYNS_MESSAGE_ENTRY(std::string, method, "Entrypoint name")
BRAYNS_MESSAGE_ENTRY(ErrorDescriptionMessage, error, "Error description")
BRAYNS_MESSAGE_END()

/**
 * @brief Progress info used in progress messages.
 *
 */
BRAYNS_MESSAGE_BEGIN(ProgressInfoMessage)
BRAYNS_MESSAGE_ENTRY(std::string, id, "Request ID")
BRAYNS_MESSAGE_ENTRY(std::string, operation, "Operation description")
BRAYNS_MESSAGE_ENTRY(double, amount, "Progress percentage [0-1]")
BRAYNS_MESSAGE_END()

/**
 * @brief Progress message sent during a request processing.
 *
 */
BRAYNS_MESSAGE_BEGIN(ProgressMessage)
BRAYNS_MESSAGE_ENTRY(std::string, jsonrpc, "Protocol version")
BRAYNS_MESSAGE_ENTRY(std::string, method, "Entrypoint name")
BRAYNS_MESSAGE_ENTRY(ProgressInfoMessage, params, "Progression info")
BRAYNS_MESSAGE_END()

/**
 * @brief Message send spontaneously by brayns.
 *
 */
BRAYNS_MESSAGE_BEGIN(NotificationMessage)
BRAYNS_MESSAGE_ENTRY(std::string, jsonrpc, "Protocol version")
BRAYNS_MESSAGE_ENTRY(std::string, method, "Entrypoint name")
BRAYNS_MESSAGE_ENTRY(JsonValue, params, "Message content")
BRAYNS_MESSAGE_END()

/**
 * @brief Empty message when no params or result is needed.
 *
 */
BRAYNS_MESSAGE_BEGIN(EmptyMessage)
BRAYNS_MESSAGE_END()

/**
 * @brief Serialize empty message as empty object.
 *
 */
template <>
struct JsonAdapter<EmptyMessage>
{
    static JsonSchema getSchema(const EmptyMessage&) { return {}; }

    static bool serialize(const EmptyMessage& value, JsonValue& json)
    {
        json = Poco::makeShared<JsonObject>();
        return true;
    }

    static bool deserialize(const JsonValue& json, EmptyMessage& value)
    {
        return true;
    }
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