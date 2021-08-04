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

#include "NetworkManager.h"

#include <brayns/network/context/NetworkContext.h>
#include <brayns/network/interface/ServerInterface.h>
#include <brayns/network/stream/StreamManager.h>

#include "EntrypointRegistry.h"

namespace
{
using namespace brayns;
class MessageBuilder
{
public:
    static RequestMessage build(const InputPacket& packet)
    {
        auto message = _parse(packet);
        _validateHeader(message);
        return message;
    }

private:
    static RequestMessage _parse(const InputPacket& packet)
    {
        if (!packet.isText())
        {
            throw EntrypointException("Text frame expected");
        }
        auto& json = packet.getData();
        try
        {
            return Json::parse<RequestMessage>(json);
        }
        catch (const Poco::JSON::JSONException& e)
        {
            throw EntrypointException("Failed to parse JSON request: " +
                                      e.displayText());
        }
    }

    static void _validateHeader(const RequestMessage& message)
    {
        if (message.jsonrpc != "2.0")
        {
            throw EntrypointException("Unsupported JSON-RPC version: '" +
                                      message.jsonrpc + "'");
        }
    }
};

class RequestManager
{
public:
    RequestManager(NetworkContext& context)
        : _context(&context)
    {
    }

    void processRequest(const ConnectionHandle& handle,
                        const InputPacket& packet)
    {
        if (packet.isBinary())
        {
            _processBinaryRequest(handle, packet);
            return;
        }
        if (packet.isText())
        {
            _processTextRequest(handle, packet);
            return;
        }
        BRAYNS_ERROR << "Invalid packet received.\n";
    }

private:
    void _processBinaryRequest(const ConnectionHandle& handle,
                               const InputPacket& packet)
    {
        throw std::runtime_error("Binary frames not supported");
    }

    void _processTextRequest(const ConnectionHandle& handle,
                             const InputPacket& packet)
    {
        auto request = _createRequest(handle);
        try
        {
            _dispatch(request, packet);
        }
        catch (...)
        {
            request.error(std::current_exception());
        }
    }

    NetworkRequest _createRequest(const ConnectionHandle& handle)
    {
        auto& connections = _context->getConnections();
        return {handle, connections};
    }

    void _dispatch(NetworkRequest& request, const InputPacket& packet)
    {
        auto message = MessageBuilder::build(packet);
        request.setMessage(std::move(message));
        auto& entrypoints = _context->getEntrypoints();
        entrypoints.processRequest(request);
    }

    NetworkContext* _context;
};

class ConnectionCallbacks
{
public:
    static void setup(NetworkContext& context)
    {
        auto& connections = context.getConnections();
        connections.onConnect([&](const auto& handle)
                              { onConnect(context, handle); });
        connections.onDisconnect([&](const auto& handle)
                                 { onDisconnect(context, handle); });
        connections.onRequest([&](const auto& handle, const auto& packet)
                              { onRequest(context, handle, packet); });
    }

private:
    static void onConnect(NetworkContext& context,
                          const ConnectionHandle& handle)
    {
        BRAYNS_INFO << "New connection: " << handle.getId() << ".\n";
    }

    static void onDisconnect(NetworkContext& context,
                             const ConnectionHandle& handle)
    {
        auto& tasks = context.getTasks();
        tasks.disconnect(handle);
        BRAYNS_INFO << "Connection closed: " << handle.getId() << ".\n";
    }

    static void onRequest(NetworkContext& context,
                          const ConnectionHandle& handle,
                          const InputPacket& packet)
    {
        RequestManager manager(context);
        manager.processRequest(handle, packet);
    }
};
} // namespace

namespace brayns
{
NetworkManager::NetworkManager() {}

NetworkManager::~NetworkManager()
{
    if (_api->getActionInterface() != _interface.get())
    {
        return;
    }
    _api->setActionInterface(nullptr);
}

void NetworkManager::init()
{
    _context = std::make_unique<NetworkContext>(*_api);
    _interface = std::make_shared<ServerInterface>(*_context);
    _api->setActionInterface(_interface);
    EntrypointRegistry::registerEntrypoints(*_interface);
    ConnectionCallbacks::setup(*_context);
}

void NetworkManager::preRender()
{
    auto& connections = _context->getConnections();
    connections.update();
    auto& tasks = _context->getTasks();
    tasks.update();
    auto& binary = _context->getBinary();
    binary.update();
}

void NetworkManager::postRender()
{
    auto& entrypoints = _context->getEntrypoints();
    entrypoints.update();
    auto& stream = _context->getStream();
    stream.update();
}
} // namespace brayns
