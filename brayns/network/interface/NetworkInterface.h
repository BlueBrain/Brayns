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

#include <brayns/network/entrypoint/EntrypointRef.h>
#include <brayns/network/socket/NetworkSocket.h>

#include "ActionInterface.h"

namespace brayns
{
class NetworkContext;

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
     * @brief Construct an interface with context access.
     *
     * @param context Network context reference.
     */
    NetworkInterface(NetworkContext& context);

    /**
     * @brief Receive requests and send replies to the client until the
     * connection is closed.
     *
     * Can be used by child class when a new connection is opened to delegate
     * clients management.
     *
     * @param socket Socket used for communication.
     */
    void run(NetworkSocketPtr socket);

    /**
     * @brief Register an entrypoint.
     *
     * @param entrypoint Entrypoint to register.
     */
    virtual void addEntrypoint(EntrypointRef entrypoint) override;

    /**
     * @brief Run setup on all registered entrypoints.
     *
     */
    virtual void setupEntrypoints() override;

    /**
     * @brief Process incoming network requests.
     *
     */
    virtual void processRequests() override;

    /**
     * @brief Update all entrypoints.
     *
     */
    virtual void update() override;

private:
    NetworkContext* _context;
};
} // namespace brayns