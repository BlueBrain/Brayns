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
#include <brayns/network/entrypoint/EntrypointException.h>
#include <brayns/network/entrypoint/EntrypointSchema.h>
#include <brayns/network/entrypoint/IEntrypoint.h>
#include <brayns/network/interface/NetworkInterface.h>
#include <brayns/network/message/JsonSchemaValidator.h>
#include <brayns/network/message/MessageFactory.h>
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
            throw EntrypointException("Text frame expected");
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
            throw EntrypointException("Failed to parse JSON request: " +
                                      e.displayText());
        }
        if (message.jsonrpc != "2.0")
        {
            throw EntrypointException("Unsupported JSON-RPC version: '" +
                                      message.jsonrpc + "'");
        }
        if (message.id.empty())
        {
            throw EntrypointException("Missing message ID");
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
        catch (EntrypointException& e)
        {
            request.error(e.getCode(), e.what());
        }
        catch (ConnectionClosedException& e)
        {
            BRAYNS_DEBUG << "Transaction finished: " << e.what() << '\n';
            return false;
        }
        catch (std::exception& e)
        {
            request.error(0, e.what());
        }
        return true;
    }

private:
    static void _run(const NetworkInterface& interface, NetworkRequest& request)
    {
        auto& method = request.getMethod();
        auto entrypoint = interface.findEntrypoint(method);
        if (!entrypoint)
        {
            throw EntrypointException("Invalid entrypoint: '" + method + "'");
        }
        auto& schema = entrypoint->getSchema();
        _validateSchema(schema, request);
        entrypoint->processRequest(request);
    }

    static void _validateSchema(const EntrypointSchema& schema,
                                NetworkRequest& request)
    {
        if (schema.params.empty())
        {
            return;
        }
        auto& schemaParams = schema.params[0];
        if (JsonSchemaInfo::isEmpty(schemaParams))
        {
            return;
        }
        auto& params = request.getParams();
        auto errors = JsonSchemaValidator::validate(params, schemaParams);
        if (!errors.isEmpty())
        {
            throw EntrypointException(errors.toString());
        }
    }
};
} // namespace

namespace brayns
{
NetworkInterface::NetworkInterface(PluginAPI& api)
    : _api(&api)
{
}

const EntrypointHolder* NetworkInterface::findEntrypoint(
    const std::string& name) const
{
    auto i = _entrypoints.find(name);
    return i == _entrypoints.end() ? nullptr : &i->second;
}

void NetworkInterface::run(NetworkSocket& socket)
{
    _clients.add(socket);
    while (NetworkTransaction::run(*this, socket))
    {
    }
    _clients.remove(socket);
}

void NetworkInterface::addEntrypoint(EntrypointPtr entrypoint)
{
    assert(entrypoint);
    entrypoint->setApi(*_api);
    entrypoint->setClientList(_clients);
    entrypoint->onCreate();
    auto name = entrypoint->getName();
    assert(!name.empty());
    _entrypoints[name] = std::move(entrypoint);
}

} // namespace brayns