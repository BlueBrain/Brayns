/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include <stdexcept>
#include <string>

#include <brayns/network/client/ClientRef.h>
#include <brayns/network/websocket/IWebSocket.h>

struct OutputPacket
{
    bool binary = false;
    std::string data;
};

class MockWebSocket : public brayns::IWebSocket
{
public:
    MockWebSocket(size_t id = 0, std::vector<brayns::InputPacket> send = {}):
        _id(id),
        _send(std::move(send))
    {
    }

    bool isClosed() const
    {
        return _closed;
    }

    const std::vector<OutputPacket> &getReceivedPackets() const
    {
        return _received;
    }

    virtual size_t getId() const override
    {
        return _id;
    }

    virtual void close() override
    {
        _closed = true;
    }

    virtual brayns::InputPacket receive() override
    {
        if (_send.empty())
        {
            throw brayns::ConnectionClosedException("Out of packets to send");
        }
        auto packet = std::move(_send.back());
        _send.pop_back();
        return packet;
    }

    virtual void send(const brayns::OutputPacket &packet) override
    {
        _received.push_back({packet.isBinary(), std::string(packet.getData())});
    }

private:
    size_t _id;
    std::vector<brayns::InputPacket> _send;
    std::vector<OutputPacket> _received;
    bool _closed = false;
};
