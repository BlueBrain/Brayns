/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include "ServerSocket.h"

#include <stdexcept>

#include <Poco/Net/Context.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/SecureServerSocket.h>
#include <Poco/Net/WebSocket.h>
#include "Poco/Net/NetException.h"

#include <brayns/utils/Log.h>

#include <brayns/network/websocket/WebSocket.h>

namespace
{
class HealthcheckHandler : public Poco::Net::HTTPRequestHandler
{
public:
    virtual void handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) override
    {
        (void)request;
        brayns::Log::debug("Healthcheck.");
        response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_OK);
        auto &stream = response.send();
        stream << "Ok";
    }
};

class WebSocketHandler : public Poco::Net::HTTPRequestHandler
{
public:
    explicit WebSocketHandler(brayns::SocketManager &manager):
        _manager(manager)
    {
    }

    virtual void handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) override
    {
        try
        {
            auto poco = _upgrade(request, response);
            auto socket = std::make_shared<brayns::WebSocket>(poco);
            auto client = brayns::ClientRef(std::move(socket));
            _manager.run(client);
        }
        catch (const Poco::Exception &e)
        {
            brayns::Log::error("Error in websocket handler: {}.", e.displayText());
        }
        catch (const std::exception &e)
        {
            brayns::Log::error("Unexpected error in websocket handler: {}.", e.what());
        }
        catch (...)
        {
            brayns::Log::error("Unknown error in websocket handler.");
        }
    }

private:
    brayns::SocketManager &_manager;

    Poco::Net::WebSocket _upgrade(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response)
    {
        try
        {
            return Poco::Net::WebSocket(request, response);
        }
        catch (const Poco::Net::WebSocketException &e)
        {
            _badRequest(e.displayText(), response);
            throw;
        }
        catch (...)
        {
            _internalError("Unexpected websocket handshake failure", response);
            throw;
        }
    }

    void _badRequest(const std::string &message, Poco::Net::HTTPServerResponse &response)
    {
        response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
        auto &stream = response.send();
        stream << message;
    }

    void _internalError(const std::string &message, Poco::Net::HTTPServerResponse &response)
    {
        response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
        auto &stream = response.send();
        stream << message;
    }
};

class RequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
    explicit RequestHandlerFactory(brayns::SocketManager &manager):
        _manager(manager)
    {
    }

    virtual Poco::Net::HTTPRequestHandler *createRequestHandler(const Poco::Net::HTTPServerRequest &request) override
    {
        brayns::Log::debug("HTTP request from '{}'.", request.getHost());
        if (request.getURI() == "/healthz")
        {
            return new HealthcheckHandler();
        }
        return new WebSocketHandler(_manager);
    }

private:
    brayns::SocketManager &_manager;
};

class SslServerContextFactory
{
public:
    static Poco::Net::Context::Ptr create(const brayns::NetworkParameters &parameters)
    {
        auto usage = Poco::Net::Context::TLS_SERVER_USE;
        auto params = Poco::Net::Context::Params();
        params.verificationMode = Poco::Net::Context::VERIFY_NONE;
        params.caLocation = parameters.getCALocation();
        params.certificateFile = parameters.getCertificateFile();
        auto context = Poco::makeAuto<Poco::Net::Context>(usage, params);
        auto privateKeyFile = parameters.getPrivateKeyFile();
        auto privateKeyPassphrase = parameters.getPrivateKeyPassphrase();
        auto privateKey = Poco::Crypto::EVPPKey("", privateKeyFile, privateKeyPassphrase);
        context->usePrivateKey(privateKey);
        return context;
    }
};

class ServerSocketFactory
{
public:
    static Poco::Net::ServerSocket create(const brayns::NetworkParameters &parameters)
    {
        auto &uri = parameters.getUri();
        auto secure = parameters.isSecure();
        auto address = Poco::Net::SocketAddress(uri);
        if (!secure)
        {
            return Poco::Net::ServerSocket(address);
        }
        auto context = SslServerContextFactory::create(parameters);
        auto backlog = 64;
        return Poco::Net::SecureServerSocket(address, backlog, context);
    }
};

class ServerParamsFactory
{
public:
    static Poco::Net::HTTPServerParams::Ptr create(const brayns::NetworkParameters &parameters)
    {
        auto maxClients = parameters.getMaxClients();
        size_t max = std::numeric_limits<int>::max();
        if (maxClients > max)
        {
            throw std::invalid_argument("Max client cannot be above 2**31");
        }
        auto handlerCount = static_cast<int>(maxClients);
        auto settings = Poco::makeAuto<Poco::Net::HTTPServerParams>();
        settings->setMaxThreads(handlerCount);
        settings->setMaxQueued(handlerCount);
        return settings;
    }
};

class ServerFactory
{
public:
    static std::unique_ptr<Poco::Net::HTTPServer> create(
        const brayns::NetworkParameters &parameters,
        brayns::SocketManager &manager)
    {
        auto factory = Poco::makeShared<RequestHandlerFactory>(manager);
        auto socket = ServerSocketFactory::create(parameters);
        auto params = ServerParamsFactory::create(parameters);
        return std::make_unique<Poco::Net::HTTPServer>(factory, socket, params);
    }
};
} // namespace

namespace brayns
{
ServerSocket::ServerSocket(const NetworkParameters &parameters, std::unique_ptr<ISocketListener> listener):
    _manager(std::move(listener))
{
    try
    {
        Log::info("Server socket initialization.");
        _server = ServerFactory::create(parameters, _manager);
    }
    catch (const Poco::Exception &e)
    {
        throw std::runtime_error("Cannot initialize server socket: '" + e.displayText() + "'");
    }
}

void ServerSocket::start()
{
    try
    {
        Log::info("Starting server socket.");
        _server->start();
    }
    catch (const Poco::Exception &e)
    {
        throw std::runtime_error("Cannot start server: '" + e.displayText() + "'");
    }
    auto &socket = _server->socket();
    auto address = socket.address();
    Log::info("Server started on '{}'.", address.toString());
}

void ServerSocket::stop()
{
    try
    {
        Log::info("Stopping server socket.");
        _server->stopAll(true);
    }
    catch (const std::exception &e)
    {
        Log::error("Failed to stop server: '{}'.", e.what());
    }
}
} // namespace brayns
