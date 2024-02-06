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

namespace brayns
{
/**
 * @brief Output data packet to send data through a WebSocket.
 *
 * Just a view object, don't take ownership or copy data.
 *
 */
class OutputPacket
{
public:
    /**
     * @brief Construct a binary packet.
     *
     * @param data The binary content of the packet.
     */
    static OutputPacket fromBinary(std::string_view data);

    /**
     * @brief Construct a text packet.
     *
     * @param data The text content of the packet.
     */
    static OutputPacket fromText(std::string_view data);

    /**
     * @brief Construct a packet with data and flags.
     *
     * @param data Packet data.
     * @param flags Flags (text or binary).
     */
    OutputPacket(std::string_view data, int flags);

    /**
     * @brief Get the data to send.
     *
     * @return std::string_view Data to send.
     */
    std::string_view getData() const;

    /**
     * @brief Get the raw flags of the packet.
     *
     * @return int Raw flags containing packet info.
     */
    int getFlags() const;

    /**
     * @brief Check if the packet is a binary frame.
     *
     * @return true Binary frame.
     * @return false Other.
     */
    bool isBinary() const;

    /**
     * @brief Check if the packet is a text frame.
     *
     * @return true Text frame
     * @return false Other.
     */
    bool isText() const;

private:
    std::string_view _data;
    int _flags = 0;
};
} // namespace brayns
