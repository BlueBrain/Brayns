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

#include "ClientInterface.h"

#include <atomic>
#include <future>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>

#include <Poco/Net/Context.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/InvalidCertificateHandler.h>
#include <Poco/Net/PrivateKeyPassphraseHandler.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/Net/SecureStreamSocket.h>

#include <brayns/common/Log.h>

#include <brayns/parameters/ParametersManager.h>

#include <brayns/network/context/NetworkContext.h>

namespace
{
using namespace brayns;

class PassphraseHandler : public Poco::Net::PrivateKeyPassphraseHandler
{
public:
    PassphraseHandler(const NetworkParameters& parameters)
        : Poco::Net::PrivateKeyPassphraseHandler(false)
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
        : Poco::Net::InvalidCertificateHandler(false)
        , _parameters(&parameters)
    {
    }

    virtual void onInvalidCertificate(
        const void*, Poco::Net::VerificationErrorArgs& args) override
    {
        Log::error("Invalid certificate: {}.", args.errorMessage());
    }

private:
    const NetworkParameters* _parameters;
};

class ClientSslContext
{
public:
    static Poco::Net::Context::Ptr create(const NetworkParameters& parameters)
    {
        auto usage = Poco::Net::Context::TLS_CLIENT_USE;
        Poco::Net::Context::Params params;
        params.privateKeyFile = parameters.getPrivateKeyFile();
        params.certificateFile = parameters.getCertificateFile();
        params.caLocation = parameters.getCALocation();
        return Poco::makeAuto<Poco::Net::Context>(usage, params);
    }
};

class ClientSslManager
{
public:
    ClientSslManager(const NetworkParameters& parameters)
    {
        auto& manager = Poco::Net::SSLManager::instance();
        auto passphrase = Poco::makeShared<PassphraseHandler>(parameters);
        auto certificate = Poco::makeShared<CertificateHandler>(parameters);
        auto context = ClientSslContext::create(parameters);
        manager.initializeClient(passphrase, certificate, context);
    }
};

class ClientSession
{
public:
    static std::unique_ptr<Poco::Net::HTTPClientSession> create(
        const NetworkParameters& parameters)
    {
        auto& uri = parameters.getUri();
        auto secure = parameters.isSecure();
        auto address = Poco::Net::SocketAddress(uri);
        if (secure)
        {
            static const ClientSslManager sslManager(parameters);
            auto host = address.host();
            auto ip = host.toString();
            auto port = address.port();
            return std::make_unique<Poco::Net::HTTPSClientSession>(ip, port);
        }
        return std::make_unique<Poco::Net::HTTPClientSession>(address);
    }
};

class ClientManager
{
public:
    static void run(const NetworkParameters& parameters,
                    NetworkInterface& interface)
    {
        auto session = ClientSession::create(parameters);
        Log::info("Establishing client session with '{}:{}'.",
                  session->getHost(), session->getPort());
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_1_1);
        Poco::Net::HTTPResponse response;
        auto socket =
            std::make_shared<NetworkSocket>(*session, request, response);
        Log::info("Client session connected.");
        interface.run(std::move(socket));
    }
};
} // namespace

namespace brayns
{
class ClientTask
{
public:
    ClientTask(const NetworkParameters& parameters, NetworkInterface& interface)
        : _parameters(&parameters)
        , _interface(&interface)
    {
    }

    void start()
    {
        _handle = std::async(std::launch::async, [this] { _run(); });
    }

    ~ClientTask()
    {
        if (!_handle.valid() || !_running)
        {
            return;
        }
        _running = false;
        try
        {
            _handle.get();
        }
        catch (const std::exception& e)
        {
            _error(e.what());
        }
    }

private:
    void _run()
    {
        _running = true;
        while (_running)
        {
            try
            {
                ClientManager::run(*_parameters, *_interface);
            }
            catch (const Poco::Exception& e)
            {
                _error(e.displayText().c_str());
            }
            catch (const std::exception& e)
            {
                _error(e.what());
            }
            auto reconnectionPeriod = _parameters->getReconnectionPeriod();
            std::this_thread::sleep_for(reconnectionPeriod);
        }
    }

    void _error(const char* message)
    {
        Log::error("Error in websocket client: {}.", message);
    }

    const NetworkParameters* _parameters;
    NetworkInterface* _interface;
    std::atomic_bool _running{false};
    std::future<void> _handle;
};

ClientInterface::ClientInterface(NetworkContext& context)
    : NetworkInterface(context)
{
    auto& api = context.getApi();
    auto& manager = api.getParametersManager();
    auto& parameters = manager.getNetworkParameters();
    _task = std::make_unique<ClientTask>(parameters, *this);
}

ClientInterface::~ClientInterface() {}

void ClientInterface::start()
{
    _task->start();
}
} // namespace brayns