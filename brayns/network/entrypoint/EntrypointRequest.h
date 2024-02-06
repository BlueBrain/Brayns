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

#include <brayns/network/jsonrpc/JsonRpcRequest.h>

namespace brayns
{
/**
 * @brief Wrapper around a network request strongly typed.
 *
 * @tparam ParamsType Type of request params.
 * @tparam ResultType Type of request result.
 */
template<typename ParamsType, typename ResultType>
class EntrypointRequest
{
public:
    /**
     * @brief Construct a request wrapping the generic (JSON) request.
     *
     * @param request Request to wrap.
     */
    explicit EntrypointRequest(JsonRpcRequest request):
        _request(std::move(request))
    {
    }

    /**
     * @brief Get the client sending the request.
     *
     * @return const ClientRef& Client ref.
     */
    const ClientRef &getClient() const
    {
        return _request.getClient();
    }

    /**
     * @brief Get the JSON-RPC ID of the request (int, string, null).
     *
     * @return const RequestId& Request ID.
     */
    const RequestId &getId() const
    {
        return _request.getId();
    }

    /**
     * @brief Get the JSON-RPC method of the request.
     *
     * @return const std::string& Method name.
     */
    const std::string &getMethod() const
    {
        return _request.getMethod();
    }

    /**
     * @brief Get the JSON-RPC params of the request at JSON format.
     *
     * @return const JsonValue& JSON request params.
     */
    const JsonValue &getJsonParams() const
    {
        return _request.getParams();
    }

    /**
     * @brief Parse the JSON-RPC params of the request parsed as ParamsType.
     *
     * @return ParamsType Request parsed params.
     */
    ParamsType getParams() const
    {
        auto &json = _request.getParams();
        return Json::deserialize<ParamsType>(json);
    }

    /**
     * @brief Parse the params of the request to the given ParamsType instance.
     *
     * @param params Parsing destination.
     */
    void getParams(ParamsType &params) const
    {
        auto &json = _request.getParams();
        Json::deserialize(json, params);
    }

    /**
     * @brief Get the Binary data of the request if any.
     *
     * @return const std::string& Binary data (can be empty).
     */
    const std::string &getBinary() const
    {
        return _request.getBinary();
    }

    /**
     * @brief Send a success reply with the given result.
     *
     * @param result Result data of the reply.
     */
    void reply(const ResultType &result) const
    {
        auto json = Json::serialize(result);
        _request.reply(json);
    }

    /**
     * @brief Send a success reply with the given result and binary data.
     *
     * @param result Result field of the reply.
     * @param binary Binary data of the reply.
     */
    void reply(const ResultType &result, std::string_view binary) const
    {
        auto json = Json::serialize(result);
        _request.reply(json, binary);
    }

    /**
     * @brief Send an error message in case of exception.
     *
     * @param e Source of the error.
     */
    void error(const JsonRpcException &e) const
    {
        _request.error(e);
    }

    /**
     * @brief Send a progress message of the request to the client.
     *
     * @param operation Operation description.
     * @param amount Operation progress amount 0-1.
     */
    void progress(const std::string &operation, double amount) const
    {
        _request.progress(operation, amount);
    }

private:
    JsonRpcRequest _request;
};
} // namespace brayns
