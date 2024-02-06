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

#include <Poco/Buffer.h>

namespace brayns
{
/**
 * @brief Input data packet received from a WebSocket.
 *
 */
class InputPacket
{
public:
    /**
     * @brief Create a binary frame.
     *
     * @param data Packet binary data.
     * @return InputPacket Binary frame.
     */
    static InputPacket fromBinary(std::string_view data);

    /**
     * @brief Create a text frame.
     *
     * @param data Packet text data.
     * @return InputPacket Text frame.
     */
    static InputPacket fromText(std::string_view data);

    /**
     * @brief Construct a packet using the buffer data and the flags.
     *
     * @param data Raw data of the packet.
     * @param flags Packet info (binary, close, etc).
     */
    InputPacket(Poco::Buffer<char> data, int flags);

    /**
     * @brief Check if the packet data is empty (no flags, no data).
     *
     * @return true No data inside the packet.
     * @return false The packet is not empty.
     */
    bool isEmpty() const;

    /**
     * @brief Get the data of the packet.
     *
     * @return std::string_view Raw data of the packet.
     */
    std::string_view getData() const;

    /**
     * @brief Check if the packet content is in binary format.
     *
     * @return true The packet is a binary packet.
     * @return false The packet is not a binary packet.
     */
    bool isBinary() const;
    /**
     * @brief Check if the packet content is in text format.
     *
     * @return true The packet is a text packet.
     * @return false The packet is not a text packet.
     */
    bool isText() const;

    /**
     * @brief Check if the packet is a close packet.
     *
     * @return true The client want to close the connection.
     * @return false The packet is not a close packet.
     */
    bool isClose() const;

private:
    Poco::Buffer<char> _data = 0;
    int _flags = 0;
};
} // namespace brayns
