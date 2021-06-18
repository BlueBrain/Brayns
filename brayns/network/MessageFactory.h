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
BRAYNS_MESSAGE_ENTRY(std::string, id, "Message ID")
BRAYNS_MESSAGE_ENTRY(std::string, method, "EntryPoint name")
BRAYNS_MESSAGE_ENTRY(JsonValue, params, "Request content")
BRAYNS_MESSAGE_END()

/**
 * @brief Message used by brayns to send the reply.
 * 
 */
BRAYNS_MESSAGE_BEGIN(ReplyMessage)
BRAYNS_MESSAGE_ENTRY(std::string, jsonrpc, "Protocol version")
BRAYNS_MESSAGE_ENTRY(std::string, id, "Message ID")
BRAYNS_MESSAGE_ENTRY(std::string, method, "EntryPoint name")
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
BRAYNS_MESSAGE_ENTRY(std::string, method, "EntryPoint name")
BRAYNS_MESSAGE_ENTRY(ErrorDescriptionMessage, error, "Error description")
BRAYNS_MESSAGE_END()

/**
 * @brief Message send spontaneously by brayns.
 * 
 */
BRAYNS_MESSAGE_BEGIN(UpdateMessage)
BRAYNS_MESSAGE_ENTRY(std::string, jsonrpc, "Protocol version")
BRAYNS_MESSAGE_ENTRY(std::string, method, "EntryPoint name")
BRAYNS_MESSAGE_ENTRY(JsonValue, params, "Message content")
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
BRAYNS_MESSAGE_ENTRY(std::string, method, "EntryPoint name")
BRAYNS_MESSAGE_ENTRY(ProgressInfoMessage, params, "Progression info")
BRAYNS_MESSAGE_END()

class MessageFactory
{
public:
    static ReplyMessage createReplyMessage(const RequestMessage& request)
    {
        ReplyMessage reply;
        reply.jsonrpc = request.jsonrpc;
        reply.id = request.id;
        reply.method = request.method;
        return reply;
    }

    static ErrorMessage createErrorMessage(const RequestMessage& request)
    {
        ErrorMessage error;
        error.jsonrpc = request.jsonrpc;
        error.id = request.id;
        error.method = request.method;
        return error;
    }

    static UpdateMessage createUpdateMessage(const RequestMessage& request)
    {
        UpdateMessage update;
        update.jsonrpc = request.jsonrpc;
        update.method = request.method;
        return update;
    }

    static ProgressMessage createProgressMessage(const RequestMessage& request)
    {
        ProgressMessage progress;
        progress.jsonrpc = request.jsonrpc;
        progress.method = request.method;
        progress.params.id = request.id;
        return progress;
    }
};
} // namespace brayns