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

#include <doctest/doctest.h>

#include <brayns/core/network/socket/SocketManager.h>

#include "MockWebSocket.h"

class MockListener : public brayns::ISocketListener
{
public:
    const std::vector<brayns::ClientRef> getConnectedClients() const
    {
        return _connected;
    }

    const std::vector<brayns::ClientRef> getDisconnectedClients() const
    {
        return _disconnected;
    }

    const std::vector<brayns::ClientRequest> getReceivedRequests() const
    {
        return _requests;
    }

    virtual void onConnect(const brayns::ClientRef &client) override
    {
        _connected.push_back(client);
    }

    virtual void onDisconnect(const brayns::ClientRef &client) override
    {
        _disconnected.push_back(client);
    }

    virtual void onRequest(brayns::ClientRequest request) override
    {
        _requests.push_back(std::move(request));
    }

private:
    std::vector<brayns::ClientRef> _connected;
    std::vector<brayns::ClientRef> _disconnected;
    std::vector<brayns::ClientRequest> _requests;
};

TEST_CASE("SocketManager")
{
    auto packets = std::vector<brayns::InputPacket>(
        {brayns::InputPacket::fromBinary("binary"),
         brayns::InputPacket::fromText("text1"),
         brayns::InputPacket::fromText("text2")});

    SUBCASE("Run client until no more messages are available")
    {
        auto socket = std::make_shared<MockWebSocket>(0, packets);
        auto client = brayns::ClientRef(socket);
        auto ptr = std::make_unique<MockListener>();
        auto &listener = *ptr;
        auto manager = brayns::SocketManager(std::move(ptr));
        manager.run(client);
        auto &received = listener.getReceivedRequests();
        CHECK_EQ(received.size(), packets.size());
        for (size_t i = 0; i < packets.size(); ++i)
        {
            auto &request = received[i];
            auto &ref = packets[packets.size() - 1 - i];
            CHECK_EQ(request.getClient(), client);
            CHECK_EQ(request.isBinary(), ref.isBinary());
            CHECK_EQ(request.isText(), ref.isText());
            CHECK_EQ(request.getData(), ref.getData());
        }
    }
    SUBCASE("Connect and disconnect")
    {
        auto socket = std::make_shared<MockWebSocket>();
        auto client = brayns::ClientRef(socket);
        auto ptr = std::make_unique<MockListener>();
        auto &listener = *ptr;
        auto manager = brayns::SocketManager(std::move(ptr));
        manager.run(client);
        auto ref = std::vector<brayns::ClientRef>({client});
        CHECK_EQ(listener.getConnectedClients(), ref);
        CHECK_EQ(listener.getDisconnectedClients(), ref);
    }
}
