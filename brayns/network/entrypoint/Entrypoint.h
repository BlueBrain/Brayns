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

#include <brayns/network/interface/ActionInterface.h>
#include <brayns/network/messages/MessageFactory.h>

#include <brayns/parameters/ParametersManager.h>

#include <brayns/pluginapi/PluginAPI.h>

#include "EntrypointException.h"
#include "IEntrypoint.h"

namespace brayns
{
template <typename ParamsType, typename ResultType>
class EntrypointRequest
{
public:
    EntrypointRequest(const NetworkRequest& request)
        : _request(&request),
        _params(Json::deserialize<ParamsType>(request.getParams()))
    {
    }

    const ParamsType& getParams() const { return _params; }

    void error(int code, const std::string& message) const
    {
        _request->error(code, message);
    }

    void progress(const std::string& operation, double amount) const
    {
        _request->progress(operation, amount);
    }

    void reply(const ResultType& message) const { _request->reply(message); }

private:
    const NetworkRequest* _request;
    ParamsType _params;
};

/**
 * @brief Entrypoint common implementation used as base class.
 *
 */
template <typename ParamsType, typename ResultType>
class Entrypoint : public IEntrypoint
{
public:
    using Request = EntrypointRequest<ParamsType, ResultType>;
    using Params = ParamsType;
    using Result = ResultType;

    /**
     * @brief Get a reference to Brayns API.
     *
     * @return PluginAPI& Stored reference to Brayns API.
     */
    PluginAPI& getApi() const { return *_api; }

    /**
     * @brief Store the API reference inside instance for child reuse.
     *
     * @param api A reference to Brayns API context.
     */
    virtual void setApi(PluginAPI& api) override { _api = &api; }

    /**
     * @brief Store a reference to the client list inside the instance.
     *
     * @param clients Connected client list (don't take ownership).
     */
    virtual void setClientList(NetworkClientList& clients) override
    {
        _clients = &clients;
    }

    /**
     * @brief Build JsonSchema using JsonSchemaFactory<ParamsType>.
     *
     * @return JsonSchema Schema of the entrypoint request.
     */
    virtual JsonSchema getParamsSchema() const override
    {
        return JsonSchemaFactory<ParamsType>::createSchema();
    }

    /**
     * @brief Build JsonSchema using JsonSchemaFactory<ResultType>.
     *
     * @return JsonSchema Schema of the entrypoint reply.
     */
    virtual JsonSchema getResultSchema() const override
    {
        return JsonSchemaFactory<ResultType>::createSchema();
    }

    virtual void onRequest(const NetworkRequest& request) const override
    {
        Request entrypointRequest(request);
        onRequest(entrypointRequest);
    }

    /**
     * @brief Shortcut to process the request with the message already parsed.
     * 
     * @param request Request with socket and parsed message.
     */
    virtual void onRequest(const Request& request) const = 0;

    /**
     * @brief Broadcast a notification to all connected clients.
     *
     * @tparam MessageType Notification message type.
     * @param message Message to send (params).
     */
    template <typename MessageType>
    void notify(const MessageType& message) const
    {
        NotificationMessage notification;
        notification.jsonrpc = "2.0";
        notification.method = getName();
        notification.params = Json::serialize(message);
        auto json = Json::stringify(notification);
        _clients->broadcast(json);
    }

private:
    PluginAPI* _api = nullptr;
    NetworkClientList* _clients;
};
} // namespace brayns