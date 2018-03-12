/* Copyright (c) 2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel.Nachbaur@epfl.ch
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

#include <brayns/Brayns.h>
#include <brayns/parameters/ParametersManager.h>

#include <boost/test/unit_test.hpp>

#include <rockets/jsonrpc/client.h>
#include <rockets/ws/client.h>

#include <future>

template <typename T>
bool is_ready(const std::future<T>& f)
{
    return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

class ClientServer
{
public:
    static ClientServer& instance()
    {
        if (!_instance)
            throw std::runtime_error("Could not initialize client/server");
        return *_instance;
    }

    ClientServer(
        std::vector<const char*> additionalArgv = std::vector<const char*>())
    {
        auto& testSuite = boost::unit_test::framework::master_test_suite();
        const char* app = testSuite.argv[0];
        std::vector<const char*> argv{app, "--http-server", ":0"};
        for (const auto& arg : additionalArgv)
            argv.push_back(arg);
        const int argc = argv.size();
        brayns.reset(new brayns::Brayns(argc, argv.data()));
        brayns->loadPlugins();
        brayns->getParametersManager()
            .getApplicationParameters()
            .setImageStreamFPS(0);
        brayns->render();

        const auto uri = brayns->getParametersManager()
                             .getApplicationParameters()
                             .getHttpServerURI();

        auto connectFuture = wsClient.connect("ws://" + uri, "rockets");
        while (!is_ready(connectFuture))
        {
            wsClient.process(5);
            brayns->render();
        }
        connectFuture.get();
        _instance = this;
    }

    template <typename Params, typename RetVal>
    RetVal makeRequest(const std::string& method, const Params& params)
    {
        auto responseFuture = client.request<Params, RetVal>(method, params);
        while (!is_ready(responseFuture))
        {
            wsClient.process(0);
            brayns->render();
        }

        return responseFuture.get();
    }

    template <typename RetVal>
    RetVal makeRequest(const std::string& method)
    {
        auto responseFuture = client.request<RetVal>(method);
        while (!is_ready(responseFuture))
        {
            wsClient.process(0);
            brayns->render();
        }

        return responseFuture.get();
    }

    template <typename Params>
    void makeNotification(const std::string& method, const Params& params)
    {
        client.notify<Params>(method, params);

        wsClient.process(5);
        for (size_t i = 0; i < 10; ++i)
            brayns->render();
    }

    void makeNotification(const std::string& method)
    {
        client.notify(method, std::string());

        wsClient.process(5);
        for (size_t i = 0; i < 10; ++i)
            brayns->render();
    }

    brayns::Brayns& getBrayns() { return *brayns; }
private:
    static ClientServer* _instance;
    std::unique_ptr<brayns::Brayns> brayns;
    rockets::ws::Client wsClient;
    rockets::jsonrpc::Client<rockets::ws::Client> client{wsClient};
};

ClientServer* ClientServer::_instance{nullptr};

template <typename Params, typename RetVal>
RetVal makeRequest(const std::string& method, const Params& params)
{
    return ClientServer::instance().makeRequest<Params, RetVal>(method, params);
}

template <typename RetVal>
RetVal makeRequest(const std::string& method)
{
    return ClientServer::instance().makeRequest<RetVal>(method);
}

template <typename Params>
void makeNotification(const std::string& method, const Params& params)
{
    ClientServer::instance().makeNotification<Params>(method, params);
}

void makeNotification(const std::string& method)
{
    ClientServer::instance().makeNotification(method);
}

brayns::Camera& getCamera()
{
    return ClientServer::instance().getBrayns().getEngine().getCamera();
}
