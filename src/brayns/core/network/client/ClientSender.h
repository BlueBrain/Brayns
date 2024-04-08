/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include <string_view>

#include <brayns/core/network/client/ClientRef.h>

#include <brayns/core/json/Json.h>

namespace brayns
{
/**
 * @brief Helper class to send messages and handle connection errors.
 *
 * Log a message if the connection close during the process but do not throw.
 *
 */
class ClientSender
{
public:
    /**
     * @brief Send a binary frame with both text and binary data.
     *
     * @param text Text part of the message.
     * @param binary Binary part of the message.
     * @param client Client receiveing the frame.
     */
    static void sendRawBinary(std::string_view text, std::string_view binary, const ClientRef &client);

    /**
     * @brief Send a text frame with text only.
     *
     * @param text Message content.
     * @param client Client receiveing the frame.
     */
    static void sendRawText(std::string_view text, const ClientRef &client);

    /**
     * @brief Send a binary frame with both text and binary data.
     *
     * Message must be serializable to JSON.
     *
     * @tparam T Message type.
     * @param message Message that will be converted to text.
     * @param binary Binary part of the data frame.
     * @param client Client receiveing the frame.
     */
    template<typename T>
    static void sendBinary(const T &message, std::string_view binary, const ClientRef &client)
    {
        auto text = Json::stringify(message);
        sendRawBinary(text, binary, client);
    }

    /**
     * @brief Send a JSON message in a text frame.
     *
     * Message must be serializable to JSON.
     *
     * @tparam T Message type.
     * @param message Message that will be converted to text.
     * @param client Client receiveing the frame.
     */
    template<typename T>
    static void sendText(const T &message, const ClientRef &client)
    {
        auto text = Json::stringify(message);
        sendRawText(text, client);
    }
};
} // namespace brayns
