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

#include <memory>

#include "NetworkInterface.h"

namespace brayns
{
class ClientTask;

/**
 * @brief Client side implementation of the Network interface.
 *
 */
class ClientInterface : public NetworkInterface
{
public:
    /**
     * @brief Construct the client interface.
     *
     * Start a separated thread that creates a new connection to the server URI
     * in network parameters.
     *
     * Once a connection is established, packets are received and sent as in
     * server mode.
     *
     * If the connection is closed, the client tries to reconnect without
     * stopping the thread.
     *
     * @param context Network context reference.
     */
    ClientInterface(NetworkContext& context);

    /**
     * @brief Shutdown the client thread.
     *
     */
    virtual ~ClientInterface();

    /**
     * @brief Send the connection request to the settings server URI.
     *
     */
    virtual void start() override;

private:
    std::unique_ptr<ClientTask> _task;
};
} // namespace brayns