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

#include <functional>

#include "Connection.h"
#include "ConnectionHandle.h"

namespace brayns
{
/**
 * @brief Callback when a client connects.
 *
 */
using ConnectionCallback = std::function<void(const ConnectionHandle&)>;

/**
 * @brief Callback when a client disconnects.
 *
 */
using DisconnectionCallback = std::function<void(const ConnectionHandle&)>;

/**
 * @brief Callback when a request is received.
 *
 */
using RequestCallback =
    std::function<void(const ConnectionHandle&, const InputPacket&)>;

/**
 * @brief Pack all connection callbacks.
 *
 */
struct ConnectionListener
{
    ConnectionCallback onConnect;
    DisconnectionCallback onDisconnect;
    RequestCallback onRequest;
};
} // namespace brayns