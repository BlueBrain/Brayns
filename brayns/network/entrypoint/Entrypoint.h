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

#include <brayns/network/context/NetworkContext.h>
#include <brayns/network/message/MessageFactory.h>

#include "EntrypointException.h"
#include "IEntrypoint.h"

namespace brayns
{
template <typename ParamsType, typename ResultType>
class EntrypointRequest
{
public:
    EntrypointRequest(const NetworkRequest& request)
        : _request(&request)
        , _params(Json::deserialize<ParamsType>(request.getParams()))
    {
    }

    const ParamsType& getParams() const { return _params; }

    void error(int code, const std::string& message) const
    {
        _request->error(code, message);
    }

    void error(const std::string& message) const { _request->error(message); }

    void progress(const std::string& operation, double amount) const
    {
        _request->progress(operation, amount);
    }

    void reply(const ResultType& result) const { _request->reply(result); }

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

    /**
     * @brief Get the Network context assigned to the entrypoint.
     *
     * @return NetworkContext& Network context.
     */
    NetworkContext& getContext() const { return *_context; }

    /**
     * @brief Shortcut to get a reference to Brayns API.
     *
     * @return PluginAPI& Brayns API access.
     */
    PluginAPI& getApi() const { return _context->getApi(); }

    /**
     * @brief Shortcut to get the Entrypoint list.
     *
     * @return EntrypointManager& Entrypoint manager.
     */
    EntrypointManager& getEntrypoints() const
    {
        return _context->getEntrypoints();
    }

    /**
     * @brief Shortcut to get the connection list.
     *
     * @return ConnectionManager& Client connection manager.
     */
    ConnectionManager& getConnections() const
    {
        return _context->getConnections();
    }

    /**
     * @brief Shortcut to get the stream manager.
     *
     * @return StreamManager& Stream manager.
     */
    StreamManager& getStream() const { return _context->getStream(); }

    /**
     * @brief Store the network context reference inside instance.
     *
     * @param context A reference to the network context.
     */
    virtual void setContext(NetworkContext& context) override
    {
        _context = &context;
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
     * @brief Shortcut to trigger the rendering of a new frame.
     * 
     */
    void triggerRender() const
    {
        auto& engine = getApi().getEngine();
        engine.triggerRender();
    }

    /**
     * @brief Broadcast a notification to all connected clients.
     *
     * @tparam MessageType Notification message type.
     * @param params Message to send ("params" field).
     */
    template <typename T>
    void notify(const T& params) const
    {
        try
        {
            _sendNotification(params);
        }
        catch (std::exception& e)
        {
            BRAYNS_ERROR << "Error during notification: " << e.what() << '\n';
        }
    }

private:
    template <typename T>
    void _sendNotification(const T& params) const
    {
        NotificationMessage notification;
        notification.jsonrpc = "2.0";
        notification.method = getName();
        notification.params = Json::serialize(params);
        auto json = Json::stringify(notification);
        auto& connections = _context->getConnections();
        connections.broadcast(json);
    }

    NetworkContext* _context = nullptr;
};
} // namespace brayns