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

#include <functional>
#include <string>

#include <brayns/pluginapi/PluginAPI.h>

#include "Json.h"

namespace brayns
{
/**
 * @brief Entrypoint common interface.
 *
 */
class IEntryPoint
{
public:
    /**
     * @brief Callback set by the entrypoint manager to allow child classes to
     * send update messages to the client while running a request.
     *
     */
    using UpdateCallback = std::function<void(const JsonValue&)>;

    /**
     * @brief Default virtual destructor.
     *
     */
    virtual ~IEntryPoint() = default;

    /**
     * @brief Allow the child classes to access Brayns API. Invalid before
     * onCreate() is called.
     *
     * @return PluginApi& A reference to access Brayns API.
     */
    PluginApi& getApi() const { return *api; }

    /**
     * @brief Used by the manager to setup API reference.
     *
     * @param api A reference to Brayns API context.
     */
    void setApi(PluginAPI& api) { _api = &api; };

    /**
     * @brief Used to send an update message to the client while a request is
     * running. Invalid before onCreate() is called.
     *
     * @param message The JSON message to send.
     */
    void sendUpdateMessage(const JsonValue& message) { _callback(message); }

    /**
     * @brief Used by the manager to define how to send update messages.
     *
     * @param callback The callback triggered when the entrypoint want to send
     * an update message.
     */
    void setUpdateCallback(const UpdateCallback& callback)
    {
        _callback = callback;
    }

    /**
     * @brief Called once the entrypoint is ready to be used.
     *
     */
    virtual void onCreate() {}

    /**
     * @brief Called when the entrypoint will be destroyed.
     *
     */
    virtual void onDestroy() {}

    /**
     * @brief Must return the name of the entrypoint.
     *
     * @return std::string The name (path) of the entrypoint.
     */
    virtual std::string getName() const = 0;

    /**
     * @brief Must return a description of the entrypoint.
     *
     * @return std::string A user-define description.
     */
    virtual std::string getDescription() const = 0;

    /**
     * @brief Return the JSON schema of the request.
     *
     * @return JsonValue The request JSON schema.
     */
    virtual JsonValue getRequestSchema() const = 0;

    /**
     * @brief Return the JSON schema of the reply.
     *
     * @return JsonValue The reply JSON schema.
     */
    virtual JsonValue getReplySchema() const = 0;

    /**
     * @brief Process a JSON request sent by the client to the entrypoint.
     *
     * @param request The JSON request already validated with request schema.
     * @return JsonValue The JSON reply.
     */
    virtual JsonValue handleRequest(const JsonValue& request) const = 0;

private:
    PluginApi* _api = nullptr;
    UpdateCallback _callback;
};
} // namespace brayns