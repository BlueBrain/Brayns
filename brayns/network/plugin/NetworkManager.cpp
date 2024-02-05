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

#include "NetworkManager.h"

#include <brayns/json/JsonSchemaValidator.h>
#include <brayns/network/context/NetworkContext.h>
#include <brayns/network/interface/ClientInterface.h>
#include <brayns/network/interface/ServerInterface.h>
#include <brayns/network/stream/StreamManager.h>

#include "NetworkManagerEntrypoints.h"

namespace
{
using namespace brayns;

class MessageBuilder
{
public:
    static RequestMessage build(const std::string& data)
    {
        auto json = _parse(data);
        _validateSchema(json);
        auto message = Json::deserialize<RequestMessage>(json);
        _validateHeader(message);
        return message;
    }

private:
    static JsonValue _parse(const std::string& data)
    {
        try
        {
            return Json::parse(data);
        }
        catch (const Poco::JSON::JSONException& e)
        {
            throw EntrypointException("Failed to parse JSON request: " +
                                      e.displayText());
        }
    }

    static void _validateSchema(const JsonValue& json)
    {
        static const JsonSchema schema = Json::getSchema<RequestMessage>();
        auto errors = JsonSchemaValidator::validate(json, schema);
        if (!errors.empty())
        {
            throw EntrypointException(0, "Invalid JSON-RPC request", errors);
        }
    }

    static void _validateHeader(const RequestMessage& message)
    {
        if (message.jsonrpc != "2.0")
        {
            throw EntrypointException("Unsupported JSON-RPC version: '" +
                                      message.jsonrpc + "'");
        }
        auto& method = message.method;
        if (method.empty())
        {
            throw EntrypointException("No method provided in request");
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
        Log::error("Invalid packet received.");
    }

private:
    void _processBinaryRequest(const ConnectionHandle& handle,
                               const InputPacket& packet)
    {
        auto& binary = _context->getBinary();
        binary.processBinaryRequest(handle, packet);
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
            request.invalidRequest(std::current_exception());
        }
    }

    NetworkRequest _createRequest(const ConnectionHandle& handle)
    {
        auto& connections = _context->getConnections();
        return {handle, connections};
    }

    void _dispatch(NetworkRequest& request, const InputPacket& packet)
    {
        auto& data = packet.getData();
        auto message = MessageBuilder::build(data);
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
        connections.onConnect(
            [&](const auto& handle) { onConnect(context, handle); });
        connections.onDisconnect(
            [&](const auto& handle) { onDisconnect(context, handle); });
        connections.onRequest([&](const auto& handle, const auto& packet) {
            onRequest(context, handle, packet);
        });
    }

private:
    static void onConnect(NetworkContext& context,
                          const ConnectionHandle& handle)
    {
        Log::info("New connection: {}.", handle.getId());
    }

    static void onDisconnect(NetworkContext& context,
                             const ConnectionHandle& handle)
    {
        auto& tasks = context.getTasks();
        tasks.disconnect(handle);
        Log::info("Connection closed: {}.", handle.getId());
    }

    static void onRequest(NetworkContext& context,
                          const ConnectionHandle& handle,
                          const InputPacket& packet)
    {
        RequestManager manager(context);
        manager.processRequest(handle, packet);
    }
};

class InterfaceFactory
{
public:
    static std::shared_ptr<ActionInterface> createInterface(
        NetworkContext& context)
    {
        auto& api = context.getApi();
        auto& manager = api.getParametersManager();
        auto& parameters = manager.getNetworkParameters();
        if (parameters.isClient())
        {
            return std::make_shared<ClientInterface>(context);
        }
        return std::make_shared<ServerInterface>(context);
    }
};
} // namespace

namespace brayns
{
NetworkManager::NetworkManager()
    : ExtensionPlugin("Core")
{
}

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
    _interface = InterfaceFactory::createInterface(*_context);
    _api->setActionInterface(_interface);
    NetworkManagerEntrypoints::load(*this);
    ConnectionCallbacks::setup(*_context);
}

void NetworkManager::preRender()
{
    auto& entrypoints = _context->getEntrypoints();
    entrypoints.preRender();
}

void NetworkManager::postRender()
{
    auto& entrypoints = _context->getEntrypoints();
    entrypoints.postRender();
    auto& stream = _context->getStream();
    stream.broadcast();
}
} // namespace brayns
