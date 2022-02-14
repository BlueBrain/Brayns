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
#include <unordered_map>

#include <brayns/network/entrypoint/EntrypointRef.h>

namespace brayns
{
/**
 * @brief Holds and manage all registered entrypoints.
 *
 */
class EntrypointManager
{
public:
    /**
     * @brief Retrieve an entrypoint using its name.
     *
     * @param name Entrypoint name (ex: "set-camera").
     * @return const EntrypointRef* Entrypoint or null if not registered.
     */
    const EntrypointRef *find(const std::string &name) const;

    /**
     * @brief Return the list of the names of all entrypoints registered.
     *
     * @return std::vector<std::string> Names of registered entrypoints.
     */
    std::vector<std::string> getNames() const;

    /**
     * @brief Add a new entrypoint.
     *
     * @param entrypoint Entrypoint implementing common interface.
     * @throw std::invalid_argument Name missing or already present.
     */
    void add(EntrypointRef entrypoint);

    /**
     * @brief Setup all registered entrypoints.
     *
     */
    void onCreate();

    /**
     * @brief Dispatch request to corresponding entrypoint.
     *
     * @param request Client text request.
     * @throw JsonRpcException If an error occurs and must be replied.
     */
    void onRequest(const JsonRpcRequest &request) const;

    /**
     * @brief Update all entrypoints.
     *
     */
    void onUpdate() const;

    /**
     * @brief Notify all entrypoints before render.
     *
     */
    void onPreRender() const;

    /**
     * @brief Notify all entrypoints after render.
     *
     */
    void onPostRender() const;

private:
    std::unordered_map<std::string, EntrypointRef> _entrypoints;
};
} // namespace brayns
