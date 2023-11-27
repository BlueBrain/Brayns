/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include <atomic>
#include <future>
#include <memory>

#include <brayns/parameters/NetworkParameters.h>

#include "ISocket.h"
#include "SocketManager.h"

namespace brayns
{
/**
 * @brief Client task used to run a single client session.
 *
 */
class ClientTask
{
public:
    ClientTask(const NetworkParameters &parameters, std::unique_ptr<ISocketListener> listener);
    ~ClientTask();

    void start();
    void stop();

private:
    void _run();

    const NetworkParameters &_parameters;
    SocketManager _manager;
    std::atomic_bool _running{false};
    std::future<void> _handle;
};

/**
 * @brief Client side implementation of the Network socket.
 *
 */
class ClientSocket : public ISocket
{
public:
    /**
     * @brief Construct the client socket.
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
     * @param parameters SSL and URI parameters.
     * @param listener Listener to call on events.
     */
    ClientSocket(const NetworkParameters &parameters, std::unique_ptr<ISocketListener> listener);

    /**
     * @brief Start client task and send connection request.
     *
     */
    virtual void start() override;

    /**
     * @brief Close connection and stop client task.
     *
     */
    virtual void stop() override;

private:
    ClientTask _task;
};
} // namespace brayns
