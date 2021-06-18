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

namespace brayns
{
struct JsonSchema
{
    template <typename RequestType, typename ReplyType>
    static JsonValue from()
    {
        return {};
    }
};

struct JsonSchemaValidator
{
    static void validate(const JsonValue& schema, const JsonValue& json) {}
};

template <typename RequestType, typename ReplyType>
class BasicEntryPoint : public BaseEntryPoint
{
public:
    BasicEntryPoint() { setSchema(JsonSchema::from<RequestType, ReplyType>()); }

    virtual ReplyType processRequest(const RequestType& request) const = 0;

    virtual void processRequest(const NetworkRequest& request) const override
    {
        auto& params = request.getParams();
        JsonSchemaValidator::validate(getSchema(), params);
        auto message = Json::deserialize<RequestType>(params);
        auto reply = processRequest(message);
        request.sendReply(reply);
    }
};
} // namespace brayns