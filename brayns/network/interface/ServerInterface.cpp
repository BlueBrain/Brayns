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

#include "ServerInterface.h"

#include <sstream>
#include <stdexcept>
#include <string>

#include <Poco/Net/Context.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/InvalidCertificateHandler.h>
#include <Poco/Net/PrivateKeyPassphraseHandler.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/Net/SecureServerSocket.h>

#include <brayns/common/log.h>

#include <brayns/parameters/ParametersManager.h>

#include <brayns/network/context/NetworkContext.h>

namespace
{
using namespace brayns;

class RequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
    RequestHandler(NetworkInterface& interface)
        : _interface(&interface)
    {
    }

    virtual void handleRequest(Poco::Net::HTTPServerRequest& request,
                               Poco::Net::HTTPServerResponse& response) override
    {
        try
        {
            auto socket = std::make_shared<NetworkSocket>(request, response);
            _interface->run(std::move(socket));
        }
        catch (std::exception& e)
        {
            BRAYNS_ERROR << "Unexpected exception in request handler: "
                         << e.what() << '\n';
        }
    }

private:
    NetworkInterface* _interface;
};

class RequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
    RequestHandlerFactory(NetworkInterface& interface)
        : _interface(&interface)
    {
    }

    virtual Poco::Net::HTTPRequestHandler* createRequestHandler(
        const Poco::Net::HTTPServerRequest& request) override
    {
        BRAYNS_DEBUG << "New connection from '" << request.getHost() << "'\n";
        return new RequestHandler(*_interface);
    }

private:
    NetworkInterface* _interface;
};

class PassphraseHandler : public Poco::Net::PrivateKeyPassphraseHandler
{
public:
    PassphraseHandler(const NetworkParameters& parameters)
        : Poco::Net::PrivateKeyPassphraseHandler(true)
        , _parameters(&parameters)
    {
    }

    virtual void onPrivateKeyRequested(const void*, std::string& key) override
    {
        key = _parameters->getPrivateKeyPassphrase();
    }

private:
    const NetworkParameters* _parameters;
};

class CertificateHandler : public Poco::Net::InvalidCertificateHandler
{
public:
    CertificateHandler(const NetworkParameters& parameters)
        : Poco::Net::InvalidCertificateHandler(true)
        , _parameters(&parameters)
    {
    }

    virtual void onInvalidCertificate(
        const void*, Poco::Net::VerificationErrorArgs& args) override
    {
        BRAYNS_ERROR << "Invalid certificate: " << args.errorMessage() << '.\n';
    }

private:
    const NetworkParameters* _parameters;
};

class ServerSslContext
{
public:
    static Poco::Net::Context::Ptr create(const NetworkParameters& parameters)
    {
        auto usage = Poco::Net::Context::SERVER_USE;
        Poco::Net::Context::Params params;
        params.privateKeyFile = parameters.getPrivateKeyFile();
        params.certificateFile = parameters.getCertificateFile();
        params.caLocation = parameters.getCALocation();
        return Poco::makeAuto<Poco::Net::Context>(usage, params);
    }
};

class ServerSslManager
{
public:
    ServerSslManager(const NetworkParameters& parameters)
    {
        auto& manager = Poco::Net::SSLManager::instance();
        auto passphrase = Poco::makeShared<PassphraseHandler>(parameters);
        auto certificate = Poco::makeShared<CertificateHandler>(parameters);
        auto context = ServerSslContext::create(parameters);
        manager.initializeServer(passphrase, certificate, context);
    }
};

class ServerSocket
{
public:
    static Poco::Net::ServerSocket create(const NetworkParameters& parameters)
    {
        auto& uri = parameters.getUri();
        auto secure = parameters.isSecure();
        auto address = Poco::Net::SocketAddress(uri);
        if (secure)
        {
            static const ServerSslManager sslManager(parameters);
            return Poco::Net::SecureServerSocket(address);
        }
        return Poco::Net::ServerSocket(address);
    }
};

class ServerParams
{
public:
    static Poco::Net::HTTPServerParams::Ptr load(
        const NetworkParameters& parameters)
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
    static std::unique_ptr<Poco::Net::HTTPServer> createServer(
        PluginAPI& api, NetworkInterface& interface)
    {
        auto& manager = api.getParametersManager();
        auto& parameters = manager.getNetworkParameters();
        auto factory = Poco::makeShared<RequestHandlerFactory>(interface);
        auto socket = ServerSocket::create(parameters);
        auto params = ServerParams::load(parameters);
        return std::make_unique<Poco::Net::HTTPServer>(factory, socket, params);
    }
};
} // namespace

namespace brayns
{
ServerInterface::ServerInterface(NetworkContext& context)
    : NetworkInterface(context)
{
    auto& api = context.getApi();
    try
    {
        _server = ServerFactory::createServer(api, *this);
        _server->start();
    }
    catch (Poco::Exception& e)
    {
        throw std::runtime_error("Cannot start server: " + e.displayText());
    }
}
} // namespace brayns