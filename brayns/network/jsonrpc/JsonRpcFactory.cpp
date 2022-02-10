/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include "JsonRpcFactory.h"

namespace brayns
{
ReplyMessage JsonRpcFactory::reply(const RequestMessage &request, const JsonValue &result)
{
    ReplyMessage reply;
    reply.jsonrpc = request.jsonrpc;
    reply.id = request.id;
    reply.method = request.method;
    reply.result = result;
    return reply;
}

ErrorMessage JsonRpcFactory::error(const RequestMessage &request, const EntrypointException &e)
{
    ErrorMessage error;
    error.jsonrpc = request.jsonrpc;
    error.id = request.id;
    error.method = request.method;
    error.error.code = e.getCode();
    error.error.message = e.what();
    error.error.data = e.getData();
    return error;
}

ErrorMessage JsonRpcFactory::error(const EntrypointException &e)
{
    ErrorMessage error;
    error.jsonrpc = "2.0";
    error.id = {};
    error.method = "";
    error.error.code = e.getCode();
    error.error.message = e.what();
    error.error.data = e.getData();
    return error;
}

ProgressMessage JsonRpcFactory::progress(const RequestMessage &request, double amount, const std::string &operation)
{
    ProgressMessage progress;
    progress.jsonrpc = request.jsonrpc;
    progress.method = request.method;
    progress.params.id = request.id;
    progress.params.amount = amount;
    progress.params.operation = operation;
    return progress;
}

NotificationMessage JsonRpcFactory::notification(const std::string &method, const JsonValue &params)
{
    NotificationMessage message;
    message.jsonrpc = "2.0";
    message.method = method;
    message.params = params;
    return message;
}
} // namespace brayns
