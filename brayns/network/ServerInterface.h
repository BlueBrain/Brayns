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

#pragma once

#include <memory>

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include "NetworkInterface.h"

namespace brayns
{
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
            NetworkSocket socket(request, response);
            _interface->run(socket);
        }
        catch (std::exception& e)
        {
            std::cout << e.what() << '\n';
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
        return new RequestHandler(*_interface);
    }

private:
    NetworkInterface* _interface;
};

class ServerFactory
{
public:
    static std::unique_ptr<Poco::Net::HTTPServer> createServer(
        PluginAPI& api, NetworkInterface& interface)
    {
        return std::make_unique<Poco::Net::HTTPServer>(createFactory(interface),
                                                       getPort(api),
                                                       createSettings(api));
    }

private:
    static Poco::Net::HTTPRequestHandlerFactory::Ptr createFactory(
        NetworkInterface& interface)
    {
        return Poco::makeShared<RequestHandlerFactory>(interface);
    }

    static const std::string& getUri(PluginAPI& api)
    {
        auto& manager = api.getParametersManager();
        auto& application = manager.getApplicationParameters();
        return application.getHttpServerURI();
    }

    static Poco::UInt16 getPort(PluginAPI& api)
    {
        auto& uri = getUri(api);
        auto i = uri.find(':');
        if (i >= uri.size() - 1)
        {
            throw std::runtime_error("Invalid server URI: " + uri);
        }
        Poco::UInt16 port = 0;
        std::istringstream stream(uri.c_str() + i + 1);
        stream >> port;
        if (stream.fail())
        {
            throw std::runtime_error("Invalid server port: " + uri);
        }
        return port;
    }

    static Poco::Net::HTTPServerParams::Ptr createSettings(PluginAPI& api)
    {
        auto settings = Poco::makeAuto<Poco::Net::HTTPServerParams>();
        settings->setMaxThreads(1);
        settings->setMaxQueued(10000);
        settings->setServerName(getUri(api));
        return settings;
    }
};

class ServerInterface : public NetworkInterface
{
public:
    ServerInterface(PluginAPI& api)
        : NetworkInterface(api)
        , _server(ServerFactory::createServer(api, *this))
    {
        _server->start();
    }

private:
    std::unique_ptr<Poco::Net::HTTPServer> _server;
};
}