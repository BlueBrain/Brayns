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

#include "JsonRpcFactory.h"

namespace brayns
{
ReplyMessage JsonRpcFactory::reply(const RequestMessage &request, const JsonValue &result)
{
    ReplyMessage reply;
    reply.jsonrpc = request.jsonrpc;
    reply.id = request.id;
    reply.result = result;
    return reply;
}

ErrorMessage JsonRpcFactory::error(const RequestMessage &request, const JsonRpcException &e)
{
    ErrorMessage error;
    error.jsonrpc = request.jsonrpc;
    error.id = request.id;
    error.error.code = e.getCode();
    error.error.message = e.what();
    error.error.data = e.getData();
    return error;
}

ErrorMessage JsonRpcFactory::error(const ReplyMessage &reply, const JsonRpcException &e)
{
    ErrorMessage error;
    error.jsonrpc = reply.jsonrpc;
    error.id = reply.id;
    error.error.code = e.getCode();
    error.error.message = e.what();
    error.error.data = e.getData();
    return error;
}

ErrorMessage JsonRpcFactory::error(const JsonRpcException &e)
{
    ErrorMessage error;
    error.jsonrpc = "2.0";
    error.id = {};
    error.error.code = e.getCode();
    error.error.message = e.what();
    error.error.data = e.getData();
    return error;
}

ProgressMessage JsonRpcFactory::progress(const RequestMessage &request, const std::string &operation, double amount)
{
    ProgressMessage progress;
    progress.jsonrpc = request.jsonrpc;
    progress.method = request.method;
    progress.params.id = request.id;
    progress.params.operation = operation;
    progress.params.amount = amount;
    return progress;
}
} // namespace brayns
