/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/network/socket/NetworkRequest.h>

namespace brayns
{
/**
 * @brief Wrapper around a network request strongly typed.
 *
 * @tparam ParamsType Type of request params.
 * @tparam ResultType Type of request result.
 */
template <typename ParamsType, typename ResultType>
class EntrypointRequest
{
public:
    /**
     * @brief Construct an empty request.
     *
     */
    EntrypointRequest() = default;

    /**
     * @brief Construct a request wrapping the generic (JSON) request.
     *
     * @param request Request to wrap.
     */
    EntrypointRequest(NetworkRequest request)
        : _request(std::move(request))
    {
    }

    /**
     * @brief Get the connection handle of the client sending the request.
     *
     * @return const ConnectionHandle& Client connection handle.
     */
    const ConnectionHandle& getConnectionHandle() const
    {
        return _request.getConnectionHandle();
    }

    /**
     * @brief Get the JSON-RPC ID of the request (int, string, null).
     *
     * @return const RequestId& Request ID.
     */
    const RequestId& getId() const { return _request.getId(); }

    /**
     * @brief Get the JSON-RPC method of the request.
     *
     * @return const std::string& Method name.
     */
    const std::string& getMethod() const { return _request.getMethod(); }

    /**
     * @brief Get the JSON-RPC params of the request at JSON format.
     *
     * @return const JsonValue& JSON request params.
     */
    const JsonValue& getJsonParams() const { return _request.getParams(); }

    /**
     * @brief Parse the JSON-RPC params of the request parsed as ParamsType.
     *
     * @return ParamsType Request parsed params.
     */
    ParamsType getParams() const
    {
        auto& json = _request.getParams();
        return Json::deserialize<ParamsType>(json);
    }

    /**
     * @brief Parse the params of the request to the given ParamsType instance.
     *
     * @param params Parsing destination.
     */
    void getParams(ParamsType& params) const
    {
        auto& json = _request.getParams();
        Json::deserialize(json, params);
    }

    /**
     * @brief Send an error as reply to the client.
     *
     * @param code Error code.
     * @param message Error description.
     */
    void error(int code, const std::string& message) const
    {
        _request.error(code, message);
    }

    /**
     * @brief Shortcut to send an error reply with no code.
     *
     * @param message Error description.
     */
    void error(const std::string& message) const { _request.error(message); }

    /**
     * @brief Shortcut to send an error reply from an arbitrary exception.
     *
     * @param e Opaque exception pointer.
     */
    void error(std::exception_ptr e) const { _request.error(e); }

    /**
     * @brief Send a progress message of the request to the client.
     *
     * @param operation Operation description.
     * @param amount Operation progress amount 0-1.
     */
    void progress(const std::string& operation, double amount) const
    {
        _request.progress(operation, amount);
    }

    /**
     * @brief Send a success reply with the given result.
     *
     * @param result Result data of the reply.
     */
    void reply(const ResultType& result) const { _request.reply(result); }

    /**
     * @brief Send a notification to all other clients (not the request sender).
     *
     * @tparam T Notification params type.
     * @param params Params of the notification.
     */
    template <typename T>
    void notify(const T& params) const
    {
        _request.notify(params);
    }

private:
    NetworkRequest _request;
};
} // namespace brayns