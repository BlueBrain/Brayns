/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include "ISocketListener.h"

namespace brayns
{
/**
 * @brief Helper class to manage exchange through a websocket.
 *
 */
class SocketManager
{
public:
    /**
     * @brief Construct a manager with given listener.
     *
     * @param listener Listener to call on network events.
     */
    explicit SocketManager(std::unique_ptr<ISocketListener> listener);

    /**
     * @brief Receive and send messages until connection is closed.
     *
     * Events are buffered and will be passed to listener when poll() is called.
     *
     * @param client Client ref.
     */
    void run(const ClientRef &client);

private:
    std::unique_ptr<ISocketListener> _listener;
};
} // namespace brayns
