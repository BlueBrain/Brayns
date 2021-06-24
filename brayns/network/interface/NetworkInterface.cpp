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

#include <cassert>

#include <brayns/common/log.h>
#include <brayns/common/utils/stringUtils.h>
#include <brayns/network/entrypoint/EntryPoint.h>
#include <brayns/network/entrypoint/EntryPointException.h>
#include <brayns/network/entrypoint/EntryPointSchema.h>
#include <brayns/network/interface/NetworkInterface.h>
#include <brayns/network/messages/JsonSchemaValidator.h>
#include <brayns/network/messages/MessageFactory.h>
#include <brayns/network/socket/NetworkRequest.h>

using namespace brayns;

namespace
{
class MessageReceiver
{
public:
    static RequestMessage receive(NetworkSocket& socket)
    {
        auto packet = _receivePacket(socket);
        return _parseMessage(packet);
    }

private:
    static InputPacket _receivePacket(NetworkSocket& socket)
    {
        auto packet = socket.receive();
        if (!packet.isText())
        {
            throw EntryPointException("Text frame expected");
        }
        return packet;
    }

    static RequestMessage _parseMessage(const InputPacket& packet)
    {
        RequestMessage message;
        try
        {
            message = Json::parse<RequestMessage>(packet.getData());
        }
        catch (Poco::JSON::JSONException& e)
        {
            throw EntryPointException("Failed to parse JSON request: " +
                                      e.displayText());
        }
        if (message.jsonrpc != "2.0")
        {
            throw EntryPointException("Unsupported JSON-RPC version: " +
                                      message.jsonrpc);
        }
        if (message.id.empty())
        {
            throw EntryPointException("Missing message ID");
        }
        return message;
    }
};

class NetworkTransaction
{
public:
    static bool run(const NetworkInterface& interface, NetworkSocket& socket)
    {
        NetworkRequest request(socket);
        try
        {
            auto message = MessageReceiver::receive(socket);
            request.setMessage(message);
            _run(interface, request);
        }
        catch (EntryPointException& e)
        {
            request.sendError(e.getCode(), e.what());
        }
        catch (ConnectionClosedException& e)
        {
            BRAYNS_DEBUG << e.what() << '\n';
            return false;
        }
        catch (std::exception& e)
        {
            request.sendError(0, e.what());
        }
        return true;
    }

private:
    static void _run(const NetworkInterface& interface, NetworkRequest& request)
    {
        auto& method = request.getMethod();
        auto entryPoint = interface.findEntryPoint(method);
        if (!entryPoint)
        {
            throw EntryPointException("Invalid entrypoint: " + method);
        }
        _validateSchema(*entryPoint, request);
        entryPoint->run(request);
    }

    static void _validateSchema(const EntryPoint& entryPoint,
                                NetworkRequest& request)
    {
        auto& schema = entryPoint.getSchema();
        if (schema.params.isEmpty())
        {
            return;
        }
        auto& params = request.getParams();
        auto errors = JsonSchemaValidator::validate(params, schema.params);
        if (errors.isEmpty())
        {
            return;
        }
        throw EntryPointException("JSON schema errors:\n" + errors.toString());
    }
};
} // namespace

namespace brayns
{
NetworkInterface::NetworkInterface(PluginAPI& api)
    : _api(&api)
{
}

const EntryPoint* NetworkInterface::findEntryPoint(
    const std::string& name) const
{
    auto i = _entryPoints.find(name);
    return i == _entryPoints.end() ? nullptr : i->second.get();
}

void NetworkInterface::run(NetworkSocket& socket)
{
    while (NetworkTransaction::run(*this, socket))
    {
    }
}

void NetworkInterface::addEntryPoint(EntryPointPtr entryPoint)
{
    assert(entryPoint);
    entryPoint->setApi(*_api);
    entryPoint->init();
    auto& name = entryPoint->getName();
    assert(!name.empty());
    _entryPoints[name] = std::move(entryPoint);
}

} // namespace brayns