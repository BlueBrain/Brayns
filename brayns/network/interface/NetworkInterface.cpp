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

namespace
{
using namespace brayns;

class MessageParser
{
public:
    static RequestMessage parse(const InputPacket& packet)
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
        catch (Poco::JSON::JSONException& e)
        {
            throw EntrypointException("Failed to parse JSON request: " +
                                      e.displayText());
        }
    }
};

class MessageValidator
{
public:
    static void validateHeader(const RequestMessage& message)
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

    static void validateParams(const JsonValue& params,
                               const EntrypointSchema& schema)
    {
        if (schema.params.empty())
        {
            return;
        }
        auto& paramsSchema = schema.params[0];
        if (JsonSchemaInfo::isEmpty(paramsSchema))
        {
            return;
        }
        auto errors = JsonSchemaValidator::validate(params, paramsSchema);
        if (!errors.isEmpty())
        {
            throw EntrypointException(errors.toString());
        }
    }
};

class MessageDispatcher
{
public:
    static void dispatch(NetworkRequest& request, NetworkContext& context)
    {
        auto& method = request.getMethod();
        auto& entrypoints = context.getEntrypoints();
        auto entrypoint = entrypoints.find(method);
        if (!entrypoint)
        {
            throw EntrypointException("Invalid entrypoint: '" + method + "'");
        }
        _validateSchema(request, *entrypoint);
        auto lock = context.lock();
        entrypoint->processRequest(request);
    }

private:
    static void _validateSchema(NetworkRequest& request,
                                const EntrypointRef& entrypoint)
    {
        auto& schema = entrypoint.getSchema();
        auto& params = request.getParams();
        MessageValidator::validateParams(params, schema);
    }
};

class MessageReceiver
{
public:
    static bool receive(NetworkSocket& socket, NetworkContext& context)
    {
        NetworkRequest request(socket);
        try
        {
            _receive(socket, request);
            MessageDispatcher::dispatch(request, context);
        }
        catch (EntrypointException& e)
        {
            BRAYNS_DEBUG << "Entrypoint error: " << e.what() << '\n';
            request.error(e.getCode(), e.what());
        }
        catch (ConnectionClosedException& e)
        {
            BRAYNS_DEBUG << "Connection closed: " << e.what() << '\n';
            return false;
        }
        catch (std::exception& e)
        {
            BRAYNS_DEBUG << "Unknown error: " << e.what() << '\n';
            request.error(0, e.what());
        }
        return true;
    }

private:
    static void _receive(NetworkSocket& socket, NetworkRequest& request)
    {
        BRAYNS_DEBUG << "Waiting for client request\n";
        auto packet = socket.receive();
        BRAYNS_DEBUG << "Message received: " << packet.getData() << '\n';
        auto message = MessageParser::parse(packet);
        request.setMessage(message);
    }
};

class ClientManager
{
public:
    static void run(NetworkSocket& socket, NetworkContext& context)
    {
        _addClient(socket, context);
        while (MessageReceiver::receive(socket, context))
        {
        }
        _removeClient(socket, context);
    }

private:
    static void _addClient(NetworkSocket& socket, NetworkContext& context)
    {
        auto& clients = context.getClients();
        auto lock = context.lock();
        clients.add(socket);
    }

    static void _removeClient(NetworkSocket& socket, NetworkContext& context)
    {
        auto& clients = context.getClients();
        auto lock = context.lock();
        clients.remove(socket);
    }
};
} // namespace

namespace brayns
{
NetworkInterface::NetworkInterface(NetworkContext& context)
    : _context(&context)
{
}

void NetworkInterface::run(NetworkSocket& socket)
{
    ClientManager::run(socket, *_context);
}

void NetworkInterface::addEntrypoint(EntrypointRef entrypoint)
{
    auto lock = _context->lock();
    auto& entrypoints = _context->getEntrypoints();
    entrypoints.add(std::move(entrypoint));
}
} // namespace brayns