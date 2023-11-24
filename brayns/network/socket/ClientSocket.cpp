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

#include "ClientSocket.h"

#include <stdexcept>
#include <thread>

#include <Poco/Net/Context.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/WebSocket.h>

#include <brayns/utils/Log.h>

#include <brayns/network/websocket/WebSocket.h>

namespace
{
class SslClientContextFactory
{
public:
    static Poco::Net::Context::Ptr create(const brayns::NetworkParameters &parameters)
    {
        auto usage = Poco::Net::Context::TLS_CLIENT_USE;
        auto caLocation = parameters.getCALocation();
        auto context = Poco::makeAuto<Poco::Net::Context>(usage, caLocation);
        auto certificateFile = parameters.getCertificateFile();
        auto certificate = Poco::Crypto::X509Certificate(certificateFile);
        context->useCertificate(certificate);
        auto privateKeyFile = parameters.getPrivateKeyFile();
        auto privateKeyPassphrase = parameters.getPrivateKeyPassphrase();
        auto privateKey = Poco::Crypto::EVPPKey("", privateKeyFile, privateKeyPassphrase);
        context->usePrivateKey(privateKey);
        return context;
    }
};

class ClientSessionFactory
{
public:
    static std::unique_ptr<Poco::Net::HTTPClientSession> create(const brayns::NetworkParameters &parameters)
    {
        auto &uri = parameters.getUri();
        auto secure = parameters.isSecure();
        brayns::Log::debug("Trying to establish client session with {}.", uri);
        auto address = Poco::Net::SocketAddress(uri);
        if (!secure)
        {
            return std::make_unique<Poco::Net::HTTPClientSession>(address);
        }
        auto context = SslClientContextFactory::create(parameters);
        auto host = address.host();
        auto ip = host.toString();
        auto port = address.port();
        return std::make_unique<Poco::Net::HTTPSClientSession>(ip, port, context);
    }
};

class ClientManager
{
public:
    static void run(const brayns::NetworkParameters &parameters, brayns::SocketManager &manager)
    {
        auto session = ClientSessionFactory::create(parameters);
        brayns::Log::info("Client session established with '{}:{}'.", session->getHost(), session->getPort());
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_1_1);
        Poco::Net::HTTPResponse response;
        auto poco = Poco::Net::WebSocket(*session, request, response);
        auto socket = std::make_shared<brayns::WebSocket>(poco);
        brayns::Log::info("Client socket connected.");
        auto client = brayns::ClientRef(std::move(socket));
        manager.run(client);
    }
};
} // namespace

namespace brayns
{
ClientTask::ClientTask(const NetworkParameters &parameters, std::unique_ptr<ISocketListener> listener):
    _parameters(parameters),
    _manager(std::move(listener))
{
}

void ClientTask::start()
{
    _handle = std::async(std::launch::async, [this] { _run(); });
}

void ClientTask::stop()
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
    catch (const std::exception &e)
    {
        Log::error("Error while terminating client task: '{}'.", e.what());
    }
}

ClientTask::~ClientTask()
{
    stop();
}

void ClientTask::_run()
{
    _running = true;
    while (_running)
    {
        try
        {
            ClientManager::run(_parameters, _manager);
        }
        catch (const Poco::Exception &e)
        {
            Log::debug("Connection to server failed: '{}'.", e.displayText());
        }
        catch (const std::exception &e)
        {
            Log::error("Unexpected error in client task: '{}'.", e.what());
        }
        catch (...)
        {
            Log::error("Unknown error in client task.");
        }
        auto reconnectionPeriod = _parameters.getReconnectionPeriod();
        std::this_thread::sleep_for(reconnectionPeriod);
    }
}

ClientSocket::ClientSocket(const NetworkParameters &parameters, std::unique_ptr<ISocketListener> listener):
    _task(parameters, std::move(listener))
{
    Log::info("Client socket initialization.");
}

void ClientSocket::start()
{
    _task.start();
    Log::info("Client task started.");
}

void ClientSocket::stop()
{
    _task.stop();
    Log::info("Client task stopped.");
}
} // namespace brayns
