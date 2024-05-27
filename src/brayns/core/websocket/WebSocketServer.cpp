/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include "WebSocketServer.h"

#include <limits>

#include <fmt/format.h>

#include <Poco/Net/Context.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/NetException.h>
#include <Poco/Net/SecureServerSocket.h>
#include <Poco/Net/ServerSocket.h>

namespace
{
using brayns::Logger;
using namespace brayns::experimental;

class HealthcheckRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
    explicit HealthcheckRequestHandler(Logger &logger):
        _logger(&logger)
    {
    }

    void handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) override
    {
        (void)request;

        response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_OK);

        response.send() << "OK";

        _logger->debug("Healthcheck ok");
    }

private:
    Logger *_logger;
};

class WebSocketRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
    explicit WebSocketRequestHandler(WebSocketHandler handler, std::size_t maxFrameSize, Logger &logger):
        _handler(std::move(handler)),
        _maxFrameSize(maxFrameSize),
        _logger(&logger)
    {
    }

    void handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) override
    {
        _logger->info("Upgrading to websocket");

        auto websocket = _upgrade(request, response);

        if (!websocket)
        {
            return;
        }

        _logger->info("Upgrade complete, {} is now connected", request.getHost());

        try
        {
            _handler.handle(*websocket);
        }
        catch (...)
        {
            _logger->error("Unexpected error in websocket request handler");
            websocket->close(WebSocketStatus::UnexpectedCondition, "Internal error");
        }
    }

private:
    WebSocketHandler _handler;
    std::size_t _maxFrameSize;
    Logger *_logger;

    std::optional<WebSocket> _upgrade(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response)
    {
        try
        {
            auto websocket = Poco::Net::WebSocket(request, response);

            auto maxFrameSize = static_cast<int>(_maxFrameSize);

            websocket.setMaxPayloadSize(maxFrameSize);

            return WebSocket(websocket);
        }
        catch (const Poco::Net::WebSocketException &e)
        {
            const auto &message = e.message();

            _logger->warn("Upgrade failed: '{}'", message);

            response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
            response.send() << fmt::format("Websocket handshake failed: '{}'", message);
        }
        catch (...)
        {
            _logger->error("Upgrade failed for unknown reason");

            response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            response.send() << "Internal error during websocket handshake";
        }

        return std::nullopt;
    }
};

class RequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
    explicit RequestHandlerFactory(WebSocketHandler handler, std::size_t maxFrameSize, Logger &logger):
        _handler(std::move(handler)),
        _maxFrameSize(maxFrameSize),
        _logger(&logger)
    {
    }

    Poco::Net::HTTPRequestHandler *createRequestHandler(const Poco::Net::HTTPServerRequest &request) override
    {
        const auto &host = request.getHost();
        const auto &uri = request.getURI();

        _logger->info("New HTTP request from {} to uri '{}'", host, uri);

        if (uri == "/healthz")
        {
            return new HealthcheckRequestHandler(*_logger);
        }

        if (uri == "/")
        {
            return new WebSocketRequestHandler(_handler, _maxFrameSize, *_logger);
        }

        _logger->error("No request handlers for uri '{}'", uri);

        return nullptr;
    }

private:
    WebSocketHandler _handler;
    std::size_t _maxFrameSize;
    Logger *_logger;
};

Poco::Net::Context::Ptr createSslContext(const SslSettings &settings)
{
    auto usage = Poco::Net::Context::TLS_SERVER_USE;

    auto params = Poco::Net::Context::Params();

    params.verificationMode = Poco::Net::Context::VERIFY_NONE;
    params.caLocation = settings.caLocation;
    params.certificateFile = settings.certificateFile;

    auto context = Poco::makeAuto<Poco::Net::Context>(usage, params);

    auto privateKey = Poco::Crypto::EVPPKey("", settings.privateKeyFile, settings.privateKeyPassphrase);

    context->usePrivateKey(privateKey);

    return context;
}

Poco::Net::ServerSocket createServerSocket(const WebSocketServerSettings &settings)
{
    auto address = Poco::Net::SocketAddress(settings.host, settings.port);

    if (!settings.ssl)
    {
        return Poco::Net::ServerSocket(address);
    }

    auto backlog = 64;

    auto context = createSslContext(*settings.ssl);

    return Poco::Net::SecureServerSocket(address, backlog, context);
}

Poco::Net::HTTPServerParams::Ptr extractServerParams(const WebSocketServerSettings &settings)
{
    auto params = Poco::makeAuto<Poco::Net::HTTPServerParams>();

    params->setMaxThreads(static_cast<int>(settings.maxThreadCount));
    params->setMaxQueued(static_cast<int>(settings.queueSize));

    return params;
}
}

namespace brayns::experimental
{
WebSocketServer::WebSocketServer(std::unique_ptr<Poco::Net::HTTPServer> server):
    _server(std::move(server))
{
}

WebSocketServer::~WebSocketServer()
{
    if (_server)
    {
        _server->stopAll(true);
    }
}

WebSocketServer startWebSocketServer(const WebSocketServerSettings &settings, WebSocketHandler handler, Logger &logger)
{
    if (settings.maxFrameSize > static_cast<std::size_t>(std::numeric_limits<int>::max()))
    {
        throw std::runtime_error("Max frame size cannot be above 2 ** 31");
    }

    try
    {
        auto factory = Poco::makeShared<RequestHandlerFactory>(std::move(handler), settings.maxFrameSize, logger);

        auto socket = createServerSocket(settings);

        auto params = extractServerParams(settings);

        auto server = std::make_unique<Poco::Net::HTTPServer>(factory, socket, params);

        server->start();

        return WebSocketServer(std::move(server));
    }
    catch (const Poco::Exception &e)
    {
        throw std::runtime_error(fmt::format("Failed to start websocket server: {}", e.message()));
    }
}
}
