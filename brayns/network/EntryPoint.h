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

#include <memory>
#include <string>

#include "Json.h"

namespace brayns
{
class PluginAPI;
class NetworkRequest;
class EntryPoint;

/**
 * @brief EntryPoint holder.
 *
 */
using EntryPointPtr = std::unique_ptr<EntryPoint>;

/**
 * @brief Entrypoint common interface.
 *
 */
class EntryPoint
{
public:
    virtual ~EntryPoint() = default;

    /**
     * @brief Used by the manager to setup API reference.
     *
     * @param api A reference to Brayns API context.
     */
    virtual void setApi(PluginAPI& api) = 0;

    /**
     * @brief Called once the entrypoint is ready to be used.
     *
     */
    virtual void onCreate() {}

    /**
     * @brief Must return the name of the entrypoint.
     *
     * @return std::string The name (path) of the entrypoint.
     */
    virtual std::string getName() const = 0;

    /**
     * @brief Must return a description of the entrypoint.
     *
     * @return std::string A user-defined description.
     */
    virtual std::string getDescription() const = 0;

    /**
     * @brief Must return the JSON schema of the entrypoint.
     *
     * @return JsonValue The entrypoint JSON schema.
     */
    virtual JsonValue getSchema() const = 0;

    /**
     * @brief Called each time the client sends a request to the entrypoint.
     *
     * @param request Request received from the client.
     */
    virtual void processRequest(NetworkRequest& request) const = 0;
};
} // namespace brayns