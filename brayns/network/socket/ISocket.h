/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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
 * @brief Client or server socket.
 *
 */
class ISocket
{
public:
    virtual ~ISocket() = default;

    /**
     * @brief Used to start the network socket.
     *
     */
    virtual void start() = 0;

    /**
     * @brief Used to stop the network socket.
     *
     */
    virtual void stop() = 0;

    /**
     * @brief Used to poll the socket and trigger the listener.
     *
     */
    virtual void poll() = 0;
};
} // namespace brayns
