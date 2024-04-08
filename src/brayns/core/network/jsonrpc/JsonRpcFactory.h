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

#pragma once

#include <brayns/core/network/jsonrpc/JsonRpcException.h>

#include "JsonRpcMessages.h"

namespace brayns
{
/**
 * @brief Helper class to create standard messages.
 *
 */
class JsonRpcFactory
{
public:
    /**
     * @brief Create a ReplyMessage corresponding to a RequestMessage.
     *
     * The resulting reply message will have the same attributes as the request
     * (id, version, etc).
     *
     * @param request Request message containing the transaction info.
     * @param result "result" field of the reply message.
     * @return ReplyMessage The reply message corresponding to request.
     */
    static ReplyMessage reply(const RequestMessage &request, const JsonValue &result);

    /**
     * @brief Create an ErrorMessage corresponding to a RequestMessage.
     *
     * The resulting error message will have the same attributes as the request
     * (id, version, etc).
     *
     * @param request Request message containing the transaction info.
     * @param e Exception to fill "error" field.
     * @return ErrorMessage The error message corresponding to request.
     */
    static ErrorMessage error(const RequestMessage &request, const JsonRpcException &e);

    /**
     * @brief Create an ErrorMessage corresponding to a ReplyMessage.
     *
     * The resulting error message will have the same attributes as the request
     * (id, version, etc).
     *
     * @param request Request message containing the transaction info.
     * @param e Exception to fill "error" field.
     * @return ErrorMessage The error message corresponding to request.
     */
    static ErrorMessage error(const ReplyMessage &reply, const JsonRpcException &e);

    /**
     * @brief Create an ErrorMessage without the request message.
     *
     * Used when the request message was not parsed correctly.
     *
     * @param e Exception to fill "error" field.
     * @return ErrorMessage The error message corresponding to request.
     */
    static ErrorMessage error(const JsonRpcException &e);

    /**
     * @brief Create a ProgressMessage corresponding to a RequestMessage.
     *
     * The resulting progress message will have the same id as the request and
     * empty description and amount (to be filled with message content).
     *
     * @param request Request message containing the transaction info.
     * @param amount Progress amount.
     * @param operation Progress operation.
     * @return ProgressMessage The progress message corresponding to request.
     */
    static ProgressMessage progress(const RequestMessage &request, const std::string &operation, double amount);
};
} // namespace brayns
