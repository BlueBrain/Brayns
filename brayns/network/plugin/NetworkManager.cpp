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

#include <brayns/network/entrypoints/AnimationParametersEntrypoint.h>
#include <brayns/network/entrypoints/CameraEntrypoint.h>
#include <brayns/network/entrypoints/ImageJpegEntrypoint.h>
#include <brayns/network/entrypoints/ImageStreamingModeEntrypoint.h>
#include <brayns/network/entrypoints/RendererEntrypoint.h>
#include <brayns/network/entrypoints/SchemaEntrypoint.h>
#include <brayns/network/entrypoints/TestEntrypoint.h>
#include <brayns/network/entrypoints/TriggerJpegStreamEntrypoint.h>
#include <brayns/network/entrypoints/VersionEntrypoint.h>

namespace
{
using namespace brayns;

class EntrypointRegistry
{
public:
    static void registerEntrypoints(ActionInterface& interface)
    {
        interface.add<GetAnimationParametersEntrypoint>();
        interface.add<SetAnimationParametersEntrypoint>();
        interface.add<GetCameraEntrypoint>();
        interface.add<SetCameraEntrypoint>();
        interface.add<ImageJpegEntrypoint>();
        interface.add<TriggerJpegStreamEntrypoint>();
        interface.add<ImageStreamingModeEntrypoint>();
        interface.add<GetRendererEntrypoint>();
        interface.add<SetRendererEntrypoint>();
        interface.add<VersionEntrypoint>();
        interface.add<SchemaEntrypoint>();
        interface.add<TestEntrypoint>();
    }
};

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
        if (message.id.empty())
        {
            throw EntrypointException("Missing message ID");
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

    void processRequests()
    {
        auto& connections = _context->getConnections();
        auto buffer = connections.extractRequestBuffer();
        for (const auto& request : buffer)
        {
            processRequest(request);
        }
    }

    void processRequest(const RequestData& data)
    {
        auto request = _createRequest(data);
        try
        {
            _processRequest(request, data);
        }
        catch (const EntrypointException& e)
        {
            request.error(e.getCode(), e.what());
        }
        catch (const std::exception& e)
        {
            request.error(e.what());
        }
        catch (...)
        {
            BRAYNS_ERROR << "Unexpected failure during request processing\n";
        }
    }

private:
    NetworkRequest _createRequest(const RequestData& data)
    {
        auto& connections = _context->getConnections();
        return {data.id, connections};
    }

    void _processRequest(NetworkRequest& request, const RequestData& data)
    {
        auto message = _buildMessage(data);
        request.setMessage(std::move(message));
        _dispatchToEntrypoints(request);
    }

    RequestMessage _buildMessage(const RequestData& data)
    {
        auto& packet = data.packet;
        return MessageBuilder::build(packet);
    }

    void _dispatchToEntrypoints(const NetworkRequest& request)
    {
        auto& entrypoints = _context->getEntrypoints();
        entrypoints.processRequest(request);
    }

    NetworkContext* _context;
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
}

void NetworkManager::preRender()
{
    RequestManager manager(*_context);
    manager.processRequests();
}

void NetworkManager::postRender()
{
    auto& entrypoints = _context->getEntrypoints();
    entrypoints.update();
    auto& stream = _context->getStream();
    stream.broadcast();
}
} // namespace brayns
