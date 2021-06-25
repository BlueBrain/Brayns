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

#include "BaseEntryPoint.h"

#include <brayns/network/messages/JsonSchemaValidator.h>

namespace brayns
{
/**
 * @brief Basic entrypoint implementation.
 *
 * A basic entrypoint process a single request and returns a single reply.
 *
 * @tparam RequestType Type of the "params" object in client request.
 * @tparam ReplyType Type of the "returns" object in server reply.
 */
template <typename RequestType, typename ReplyType>
class BasicEntryPoint : public BaseEntryPoint
{
public:
    BasicEntryPoint()
    {
        EntryPointSchema schema;
        schema.params = {JsonSchemaFactory<RequestType>::createSchema()};
        schema.params[0].name = "param";
        schema.returns = JsonSchemaFactory<ReplyType>::createSchema();
        setSchema(schema);
    }

    /**
     * @brief Process the given request body.
     *
     * @param request Request content.
     * @return ReplyType Reply content.
     */
    virtual ReplyType run(const RequestType& request) const = 0;

    /**
     * @brief Process request and send reply using child implementation.
     *
     * @param request Client request.
     */
    virtual void run(const NetworkRequest& request) const override
    {
        auto& params = request.getParams();
        auto message = Json::deserialize<RequestType>(params);
        auto reply = run(message);
        request.sendReply(reply);
    }
};
} // namespace brayns