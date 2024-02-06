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

#include <brayns/json/Json.h>

#include <brayns/network/jsonrpc/JsonRpcRequest.h>

#include "EntrypointRequest.h"
#include "IEntrypoint.h"

namespace brayns
{
/**
 * @brief Helper class to handle requests params and result with template.
 *
 * @tparam ParamsType
 * @tparam ResultType
 *
 * Example:
 * @code {.cpp}
 * // MyEntrypoint.h
 * class MyEntrypoint : public Entrypoint<MyParams, MyResult>
 * {
 * public:
 *      virtual void onRequest(const Request& request) override
 *      {
 *          auto params = request.getParams();
 *          // Do stuff
 *          request.reply(result);
 *      }
 * };
 *
 * // MyPlugin.cpp in init() method
 * add<MyEntrypoint>();
 * @endcode
 *
 */
template<typename ParamsType, typename ResultType>
class Entrypoint : public IEntrypoint
{
public:
    using Params = ParamsType;
    using Result = ResultType;
    using Request = EntrypointRequest<ParamsType, ResultType>;

    /**
     * @brief Build JsonSchema using ParamsType.
     *
     * @return JsonSchema Schema of the entrypoint request.
     */
    virtual JsonSchema getParamsSchema() const override
    {
        return Json::getSchema<ParamsType>();
    }

    /**
     * @brief Build JsonSchema using ResultType.
     *
     * @return JsonSchema Schema of the entrypoint reply.
     */
    virtual JsonSchema getResultSchema() const override
    {
        return Json::getSchema<ResultType>();
    }

    /**
     * @brief Create a specialized request for Result and Params and forward it
     * to onRequest(const Request&).
     *
     * @param request Generic request.
     */
    virtual void onRequest(const JsonRpcRequest &request) override
    {
        onRequest(Request(request));
    }

    /**
     * @brief Shortcut to process a strongly typed request.
     *
     * @param request Request specialized for Params and Result.
     */
    virtual void onRequest(const Request &request) = 0;
};
} // namespace brayns
