/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
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

#include "NetworkInterface.h"

#include <brayns/common/log.h>

#include <brayns/network/context/NetworkContext.h>

namespace
{
using namespace brayns;

class MessageReceiver
{
public:
    MessageReceiver(NetworkSocketPtr socket, NetworkContext& context)
        : _socket(std::move(socket))
        , _connections(&context.getConnections())
    {
        _connections->add(_socket);
    }

    ~MessageReceiver() { _connections->remove(_socket); }

    bool receive()
    {
        try
        {
            _receive();
            return true;
        }
        catch (const ConnectionClosedException& e)
        {
            BRAYNS_DEBUG << "Connection closed: " << e.what() << '\n';
        }
        catch (const std::exception& e)
        {
            BRAYNS_DEBUG << "Unknown receive error: " << e.what() << '\n';
        }
        return false;
    }

private:
    void _receive()
    {
        BRAYNS_DEBUG << "Waiting for client request\n";
        auto packet = _socket->receive();
        BRAYNS_DEBUG << "Message received.\n";
        _connections->receive(_socket, packet);
    }

    NetworkSocketPtr _socket;
    ConnectionManager* _connections;
};
} // namespace

namespace brayns
{
NetworkInterface::NetworkInterface(NetworkContext& context)
    : _context(&context)
{
}

void NetworkInterface::run(NetworkSocketPtr socket)
{
    MessageReceiver receiver(std::move(socket), *_context);
    while (receiver.receive())
    {
    }
}

void NetworkInterface::addEntrypoint(EntrypointRef entrypoint)
{
    auto& entrypoints = _context->getEntrypoints();
    entrypoints.add(std::move(entrypoint));
}

void NetworkInterface::setupEntrypoints()
{
    auto& entrypoints = _context->getEntrypoints();
    entrypoints.setup();
}

void NetworkInterface::processRequests()
{
    auto& connections = _context->getConnections();
    connections.update();
}

void NetworkInterface::update()
{
    auto& entrypoints = _context->getEntrypoints();
    entrypoints.update();
    auto& tasks = _context->getTasks();
    tasks.poll();
    auto& binary = _context->getBinary();
    binary.pollTasks();
}
} // namespace brayns