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

#pragma once

#include <string>

#include <brayns/json/JsonSchema.h>

#include <brayns/network/client/ClientRequest.h>
#include <brayns/network/jsonrpc/JsonRpcRequest.h>

namespace brayns
{
/**
 * @brief IEntrypoint interface.
 *
 */
class IEntrypoint
{
public:
    virtual ~IEntrypoint() = default;

    /**
     * @brief Return the name of the method bound to the entrypoint.
     *
     * @return std::string Method run by the entrypoint.
     */
    virtual std::string getMethod() const = 0;

    /**
     * @brief Return a description of the entrypoint.
     *
     * @return std::string Description of the entrypoint.
     */
    virtual std::string getDescription() const = 0;

    /**
     * @brief Return the schema of the entrypoint request (params field).
     *
     * @return JsonSchema Schema of the entrypoint request.
     */
    virtual JsonSchema getParamsSchema() const = 0;

    /**
     * @brief Return the schema of the entrypoint reply (result field).
     *
     * @return JsonSchema Schema of the entrypoint reply.
     */
    virtual JsonSchema getResultSchema() const = 0;

    /**
     * @brief Called when a JSON-RPC request is received for this method.
     *
     * @param request Client request.
     */
    virtual void onRequest(const JsonRpcRequest &request) = 0;

    /**
     * @brief Enable cancellation for long running tasks.
     *
     * Default to false.
     *
     * @return true The entrypoint can be cancelled.
     * @return false The entrypoint cannot be cancelled.
     */
    virtual bool isAsync() const
    {
        return false;
    }

    /**
     * @brief Check if the entrypoint can be run in a progress call.
     *
     * Default to false.
     *
     * @return true Can be nested (example "cancel").
     * @return false Cannot be nested (recommended).
     */
    virtual bool canBeNested() const
    {
        return false;
    }

    /**
     * @brief Called once when all plugins are initialized.
     *
     */
    virtual void onCreate()
    {
    }

    /**
     * @brief Called before each render.
     *
     */
    virtual void onPreRender()
    {
    }

    /**
     * @brief Called after each render.
     *
     */
    virtual void onPostRender()
    {
    }

    /**
     * @brief Called on all entrypoints when a binary request is received.
     *
     * @param request Client binary request.
     */
    virtual void onBinary(const ClientRequest &request)
    {
        (void)request;
    }

    /**
     * @brief Called when the entrypoint is cancelled.
     *
     * Only called when isAsync() is true and entrypoint is running.
     *
     */
    virtual void onCancel()
    {
    }

    /**
     * @brief Called when a client connects.
     *
     * @param client Client ref.
     */
    virtual void onConnect(const ClientRef &client)
    {
        (void)client;
    }

    /**
     * @brief Called when a client disconnects.
     *
     * @param client Client ref.
     */
    virtual void onDisconnect(const ClientRef &client)
    {
        (void)client;
    }
};
} // namespace brayns
