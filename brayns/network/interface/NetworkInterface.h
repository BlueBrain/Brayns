/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include <brayns/network/entrypoint/EntryPoint.h>
#include <brayns/network/socket/NetworkSocket.h>
#include <brayns/pluginapi/PluginAPI.h>

#include "ActionInterface.h"

namespace brayns
{
/**
 * @brief Storage class for entrypoints.
 *
 */
class EntryPoints
{
public:
    /**
     * @brief Construct an entrypoint registry with API access.
     *
     * @param api Brayns API reference.
     */
    EntryPoints(PluginAPI& api);

    /**
     * @brief Find an entrypoint with the given name.
     *
     * @param name EntryPoint name (method in request).
     * @return const EntryPoint* Pointer to the corresponding entrypoint or null
     * if not found.
     */
    const EntryPoint* find(const std::string& name) const;

    /**
     * @brief Register and setup the given entrypoint (API access).
     *
     * @param entryPoint
     */
    void add(EntryPointPtr entryPoint);

private:
    PluginAPI* _api;
    std::unordered_map<std::string, EntryPointPtr> _entryPoints;
};

/**
 * @brief Base implementation of ActionInterface to register entrypoints.
 *
 * Registered entrypoints will be stored inside the instance and fetched when a
 * client request message is addressed with the same name (path).
 *
 * Child classes must provide a socket opened with the client and call the run
 * method on it. Requests and replies will then be received / sent until the
 * client close the connection.
 *
 */
class NetworkInterface : public ActionInterface
{
public:
    /**
     * @brief Construct an interface with Brayns API access.
     *
     * @param api Brayns API reference.
     */
    NetworkInterface(PluginAPI& api);

    /**
     * @brief Receive requests and send reply to the client until connection is
     * closed.
     *
     * @param socket Socket used for communication.
     */
    void run(NetworkSocket& socket);

    /**
     * @brief Register an entrypoint.
     *
     * @param entryPoint EntryPoint to register.
     */
    virtual void addEntryPoint(EntryPointPtr entryPoint) override;

private:
    EntryPoints _entryPoints;
};
} // namespace brayns