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

#include "MessageFactory.h"

namespace brayns
{
JsonSchema JsonAdapter<EmptyMessage>::getSchema(const EmptyMessage &value)
{
    (void)value;
    return {};
}

bool JsonAdapter<EmptyMessage>::serialize(const EmptyMessage &value, JsonValue &json)
{
    (void)value;
    json = Poco::makeShared<JsonObject>();
    return true;
}

bool JsonAdapter<EmptyMessage>::deserialize(const JsonValue &json, EmptyMessage &value)
{
    (void)json;
    (void)value;
    return true;
}

ReplyMessage MessageFactory::createReply(const RequestMessage &request)
{
    ReplyMessage reply;
    reply.jsonrpc = request.jsonrpc;
    reply.id = request.id;
    reply.method = request.method;
    return reply;
}

ErrorMessage MessageFactory::createError(const RequestMessage &request)
{
    ErrorMessage error;
    error.jsonrpc = request.jsonrpc;
    error.id = request.id;
    error.method = request.method;
    return error;
}

ProgressMessage MessageFactory::createProgress(const RequestMessage &request)
{
    ProgressMessage progress;
    progress.jsonrpc = request.jsonrpc;
    progress.method = "progress";
    progress.params.id = request.id;
    return progress;
}
} // namespace brayns
