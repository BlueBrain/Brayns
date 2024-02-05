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

#include <string>
#include <unordered_map>

#include <brayns/network/entrypoint/EntrypointRef.h>

namespace brayns
{
class NetworkContext;

/**
 * @brief Map of entrypoints indexed by name.
 *
 */
using EntrypointMap = std::unordered_map<std::string, EntrypointRef>;

/**
 * @brief Holds and manage all registered entrypoints.
 *
 */
class EntrypointManager
{
public:
    /**
     * @brief Construct a manager with context access.
     *
     * @param context Common data.
     */
    EntrypointManager(NetworkContext& context);

    /**
     * @brief Retrieve an entrypoint using its name.
     *
     * @param name Entrypoint name (ex: "set-camera").
     * @return const EntrypointRef* Entrypoint or null if not registered.
     */
    const EntrypointRef* find(const std::string& name) const;

    /**
     * @brief Add a new entrypoint.
     *
     * @param entrypoint Entrypoint implementing common interface.
     */
    void add(EntrypointRef entrypoint);

    /**
     * @brief Setup all registered entrypoints.
     *
     * Setup the context access and call onCreate method.
     *
     */
    void setup();

    /**
     * @brief Update all entrypoints.
     *
     */
    void update() const;

    /**
     * @brief Dispatch request to corresponding entrypoint.
     *
     * @param request Client text request.
     */
    void processRequest(const NetworkRequest& request) const;

    /**
     * @brief Notify all entrypoints before render.
     *
     */
    void preRender() const;

    /**
     * @brief Notify all entrypoints after render.
     *
     */
    void postRender() const;

    /**
     * @brief Return the number of registered entrypoints.
     *
     * @return size_t Number of existing entrypoints.
     */
    size_t size() const { return _entrypoints.size(); }

    /**
     * @brief Iterate over all registered entrypoints.
     *
     * @tparam FunctorType Functor type like void(const EntrypointRef&).
     * @param functor Functor instance.
     */
    template <typename FunctorType>
    void forEach(FunctorType functor) const
    {
        for (const auto& pair : _entrypoints)
        {
            auto& entrypoint = pair.second;
            functor(entrypoint);
        }
    }

private:
    NetworkContext* _context;
    EntrypointMap _entrypoints;
};
} // namespace brayns