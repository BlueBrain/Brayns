/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <brayns/common/Log.h>

namespace
{
class RequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
    RequestHandler(brayns::SocketManager &manager)
        : _manager(manager)
    {
    }

    virtual void handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) override
    {
        try
        {
            auto socket = std::make_shared<brayns::WebSocket>(request, response);
            _manager.run(socket);
        }
        catch (const Poco::Exception &e)
        {
            brayns::Log::error("Websocket server connection failed: {}.", e.displayText());
        }
        catch (const std::exception &e)
        {
            brayns::Log::error("Unexpected error during websocket server connection: {}.", e.what());
        }
        catch (...)
        {
            brayns::Log::error("Unexpected error during websocket server connection.");
        }
    }

private:
    brayns::SocketManager &_manager;
};

class RequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
    RequestHandlerFactory(brayns::SocketManager &manager)
        : _manager(manager)
    {
    }

    virtual Poco::Net::HTTPRequestHandler *createRequestHandler(const Poco::Net::HTTPServerRequest &request) override
    {
        brayns::Log::debug("HTTP request from '{}'.", request.getHost());
        return new RequestHandler(_manager);
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
        auto caLocation = parameters.getCALocation();
        auto context = Poco::makeAuto<Poco::Net::Context>(usage, caLocation);
        auto certificateFile = parameters.getCertificateFile();
        auto certificate = Poco::Crypto::X509Certificate(certificateFile);
        context->useCertificate(certificate);
        auto privateKeyFile = parameters.getPrivateKeyFile();
        auto privateKeyPassphrase = parameters.getPrivateKeyPassphrase();
        auto privateKey = Poco::Crypto::RSAKey("", privateKeyFile, privateKeyPassphrase);
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
        auto settings = Poco::makeAuto<Poco::Net::HTTPServerParams>();
        settings->setMaxThreads(maxClients);
        settings->setMaxQueued(maxClients);
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
ServerSocket::ServerSocket(const NetworkParameters &parameters, std::unique_ptr<ISocketListener> listener)
    : _manager(std::move(listener))
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

void ServerSocket::poll()
{
    _manager.poll();
}
} // namespace brayns