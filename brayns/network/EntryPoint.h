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

#include "IEntryPoint.h"

namespace brayns
{
/**
 * @brief Template base class to map template code to entrypoint interface.
 *
 * @tparam RequestType The type of the request the entrypoint handles.
 * @tparam ReplyType The type of the reply the entrypoint returns.
 */
template <typename RequestType, typename ReplyType>
class EntryPoint : public IEntryPoint
{
public:
    /**
     * @brief Must handle the request and return a reply.
     *
     * @param request The request sent by the client.
     * @return ReplyType The reply to send to the client.
     */
    virtual ReplyType handleRequest(const RequestType& request) const = 0;

    /**
     * @brief Overrides the base class method to handle the JSON serialization
     * using the Request / Reply types.
     *
     * @param json The JSON request.
     * @return JsonValue The JSON repy.
     */
    virtual JsonValue handleRequest(const JsonValue& json) const override
    {
        auto request = Json::deserialize<RequestType>(json);
        auto reply = run(request);
        return Json::serialize(reply);
    }
};
} // namespace brayns