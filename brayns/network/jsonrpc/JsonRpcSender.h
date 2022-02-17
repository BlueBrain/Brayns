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

#include <brayns/network/client/ClientManager.h>
#include <brayns/network/client/ClientRef.h>
#include <brayns/network/jsonrpc/JsonRpcMessages.h>

namespace brayns
{
/**
 * @brief Helper class to send messages and handle connection errors.
 *
 * Log a message if the connection close during the process but do not throw.
 *
 */
class JsonRpcSender
{
public:
    /**
     * @brief Send a reply to the given client.
     *
     * @param message Reply full message.
     * @param client Client ref.
     */
    static void reply(const ReplyMessage &message, const ClientRef &client);

    /**
     * @brief Send an error message to the given client.
     *
     * @param message Error message.
     * @param client Client ref.
     */
    static void error(const ErrorMessage &message, const ClientRef &client);

    /**
     * @brief Send a progress message to the given client.
     *
     * @param message Progress message.
     * @param client Client ref.
     */
    static void progress(const ProgressMessage &message, const ClientRef &client);
};
} // namespace brayns
