/* Copyright 2015-2024 Blue Brain Project/EPFL
 *
 * Responsible Authors: Daniel.Nachbaur@epfl.ch
 *                      Nadir Román Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/network/entrypoint/EntrypointRef.h>

namespace brayns
{
/**
 * Interface with the network engine.
 *
 * Can be used to register entrypoints (recieve JSON RPC request with entrypoint
 * name as method).
 *
 * All entrypoints must implement IEntrypoint.
 *
 */
class ActionInterface
{
public:
    virtual ~ActionInterface() = default;

    /**
     * @brief Register an entrypoint implementing IEntrypoint interface.
     *
     * @param entrypoint Pointer to an IEntrypoint implementation.
     */
    virtual void addEntrypoint(EntrypointRef entrypoint) = 0;

    /**
     * @brief Called once all plugins are initialized.
     *
     * Allow interface to run entrypoints setup once all plugins have been
     * loaded.
     *
     */
    virtual void setupEntrypoints() = 0;

    /**
     * @brief Activate the network interface.
     *
     * Called once all entrypoints are setup.
     *
     */
    virtual void start() = 0;

    /**
     * @brief Process incoming network requests.
     *
     */
    virtual void processRequests() = 0;

    /**
     * @brief Update all entrypoints.
     *
     */
    virtual void update() = 0;

    /**
     * @brief Shortcut to add an entrypoint from its type.
     *
     * @tparam T Concrete type of the entrypoint (subclass of IEntrypoint).
     * @tparam Args Types of the arguments to pass to the constructor of T.
     * @param args Arguments to pass to the constructor of T.
     */
    template <typename T, typename... Args>
    void add(Args&&... args)
    {
        addEntrypoint(EntrypointRef::create<T>(std::forward<Args>(args)...));
    }
};
} // namespace brayns
