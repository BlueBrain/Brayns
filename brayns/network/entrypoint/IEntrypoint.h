/* Copyright (c) 2015-2023 EPFL/Blue Brain Project
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
     * @throw JsonRpcException Will be catched and replied as error.
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
     * @brief Check if the entrypoint must be executed directly on reception.
     *
     * Default to false.
     *
     * If an entrypoint has priority, it will be executed directly on reception
     * (in the main thread). It will not be queued and can be run inside the
     * execution of another entrypoint before previously received requests have
     * been processed.
     *
     * @return true Higher priority, will not be queued.
     * @return false No special priority, executed in reception order.
     */
    virtual bool hasPriority() const
    {
        return false;
    }

    /**
     * @brief Indicate if the entrypoint is deprecated.
     *
     * Deprecated entrypoints will be removed / renamed in the next major
     * release but are still supported.
     *
     * @return true Deprecated.
     * @return false Not deprecated.
     */
    virtual bool isDeprecated() const
    {
        return false;
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
     * @brief Called when the client of the current request disconnects.
     *
     * Only called when the entrypoint is running.
     *
     */
    virtual void onDisconnect()
    {
    }
};
} // namespace brayns
