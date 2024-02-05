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

#include "NetworkInterface.h"

#include <brayns/common/Log.h>

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
            Log::debug("Connection closed: {}.", e.what());
        }
        catch (const std::exception& e)
        {
            Log::debug("Unknown receive error: {}.", e.what());
        }
        return false;
    }

private:
    void _receive()
    {
        Log::debug("Waiting for client request...");
        auto packet = _socket->receive();
        Log::debug("Message received.");
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