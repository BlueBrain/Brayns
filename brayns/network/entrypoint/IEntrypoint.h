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

#include <deque>
#include <memory>
#include <string>
#include <vector>

#include <brayns/network/messages/JsonSchema.h>
#include <brayns/network/socket/NetworkClientList.h>
#include <brayns/network/socket/NetworkRequest.h>

namespace brayns
{
class PluginAPI;

class IEntrypoint;

/**
 * @brief Entrypoint holder.
 *
 */
using EntrypointPtr = std::unique_ptr<IEntrypoint>;

/**
 * @brief IEntrypoint interface.
 *
 */
class IEntrypoint
{
public:
    virtual ~IEntrypoint() = default;

    /**
     * @brief Used by the manager to setup API reference.
     *
     * @param api A reference to Brayns API context.
     */
    virtual void setApi(PluginAPI& api) = 0;

    /**
     * @brief Used by the manager to setup client list reference.
     *
     * @param clients A reference to the list of connected clients.
     */
    virtual void setClientList(NetworkClientList& clients) = 0;

    /**
     * @brief Return the name of the entrypoint (unique ID).
     * 
     * @return std::string Name of the entrypoint.
     */
    virtual std::string getName() const = 0;

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
     * @brief Check if the entrypoint replies asynchronously.
     * 
     * @return true Async.
     * @return false Sync.
     */
    virtual bool isAsync() const { return false; }

    /**
     * @brief Called once the entrypoint is ready to be used.
     *
     */
    virtual void onCreate() {}

    /**
     * @brief Called each time the client sends a request to the entrypoint.
     *
     * @param request Client request.
     */
    virtual void onRequest(const NetworkRequest& request) const = 0;
};
} // namespace brayns