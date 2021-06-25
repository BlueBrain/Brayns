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

#include "EntryPointSchema.h"

namespace brayns
{
class EntryPoint;
class NetworkRequest;
class PluginAPI;

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
    virtual void init() {}

    /**
     * @brief Return the JSON schema of the entrypoint.
     *
     * @return const JsonSchema& The entrypoint JSON schema.
     */
    virtual const EntryPointSchema& getSchema() const = 0;

    /**
     * @brief Called each time the client sends a request to the entrypoint.
     *
     * @param request Request received from the client used to retreive request
     * info and send and receive text frames.
     */
    virtual void run(const NetworkRequest& request) const = 0;

    /**
     * @brief Shortcut to get the name of the entrypoint from the schema.
     * 
     * @return const std::string& Name of the entrypoint.
     */
    const std::string& getName() const { return getSchema().title; }
};
} // namespace brayns