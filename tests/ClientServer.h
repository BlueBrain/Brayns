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
#include <brayns/common/engine/Engine.h>
#include <brayns/common/loader/Loader.h>
#include <brayns/common/scene/Scene.h>
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

const size_t CLIENT_PROCESS_TIMEOUT = 5;  /*ms*/
const size_t SERVER_PROCESS_RETRIES = 10; /*ms*/

class ForeverLoader : public brayns::Loader
{
public:
    using brayns::Loader::Loader;
    brayns::ModelDescriptorPtr importFromBlob(brayns::Blob&&, const size_t,
                                              const size_t) final
    {
        for (;;)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            updateProgress("still not done", 0, 1);
        }
        return {};
    }

    brayns::ModelDescriptorPtr importFromFile(const std::string&, const size_t,
                                              const size_t) final
    {
        for (;;)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            updateProgress("still not done", 0, 1);
        }
        return {};
    }
};

class ClientServer
{
public:
    static ClientServer& instance()
    {
        if (!_instance)
            throw std::runtime_error("Could not initialize client/server");
        return *_instance;
    }

    ClientServer(std::vector<const char*> additionalArgv = {})
    {
        auto& testSuite = boost::unit_test::framework::master_test_suite();
        const char* app = testSuite.argv[0];
        std::vector<const char*> argv{
            app, "demo", "--http-server", "localhost:0", "--circuit-density",
            "1"};
        for (const auto& arg : additionalArgv)
            argv.push_back(arg);
        const int argc = argv.size();
        _brayns.reset(new brayns::Brayns(argc, argv.data()));
        _brayns->loadPlugins();
        _brayns->getParametersManager()
            .getApplicationParameters()
            .setImageStreamFPS(0);
        _brayns->render();

        auto& scene = _brayns->getEngine().getScene();
        scene.getLoaderRegistry().registerLoader(
            {[] { return std::set<std::string>{std::string("forever")}; },
             [&scene] { return std::make_unique<ForeverLoader>(scene); }});

        connect(_wsClient);
        _instance = this;
    }

    void connect(rockets::ws::Client& client)
    {
        const auto uri = _brayns->getParametersManager()
                             .getApplicationParameters()
                             .getHttpServerURI();

        auto connectFuture = client.connect("ws://" + uri, "rockets");
        while (!is_ready(connectFuture))
        {
            client.process(CLIENT_PROCESS_TIMEOUT);
            _brayns->render();
        }
        connectFuture.get();
    }

    template <typename Params, typename RetVal>
    RetVal makeRequest(const std::string& method, const Params& params)
    {
        auto request = _client.request<Params, RetVal>(method, params);
        while (!request.is_ready())
        {
            _wsClient.process(0);
            _brayns->render();
        }

        return request.get();
    }

    template <typename RetVal>
    RetVal makeRequest(const std::string& method, const std::string& params)
    {
        auto request = _client.request(method, params);
        while (!request.is_ready())
        {
            _wsClient.process(0);
            _brayns->render();
        }

        RetVal retVal;
        ::from_json(retVal, request.get().result);
        return retVal;
    }

    template <typename RetVal>
    RetVal makeRequest(const std::string& method)
    {
        auto request = _client.request<RetVal>(method);
        while (!request.is_ready())
        {
            _wsClient.process(0);
            _brayns->render();
        }

        return request.get();
    }

    template <typename RetVal>
    RetVal makeRequestUpdate(const std::string& method, RetVal baseObject)
    {
        auto promise = std::make_shared<std::promise<RetVal>>();
        auto callback = [promise, &baseObject](auto response) {
            if (response.isError())
                promise->set_exception(std::make_exception_ptr(
                    rockets::jsonrpc::response_error(response.error)));
            else
            {
                if (!from_json(baseObject, response.result))
                    promise->set_exception(std::make_exception_ptr(
                        rockets::jsonrpc::response_error(
                            "Response JSON conversion failed",
                            rockets::jsonrpc::ErrorCode::
                                invalid_json_response)));
                else
                    promise->set_value(std::move(baseObject));
            }
        };

        _client.request(method, "", callback);
        auto future = promise->get_future();

        while (!rockets::is_ready(future))
        {
            _wsClient.process(0);
            _brayns->render();
        }

        return future.get();
    }

    template <typename Params>
    void makeNotification(const std::string& method, const Params& params)
    {
        _client.notify<Params>(method, params);

        _wsClient.process(CLIENT_PROCESS_TIMEOUT);
        for (size_t i = 0; i < SERVER_PROCESS_RETRIES; ++i)
            _brayns->render();
    }

    void makeNotification(const std::string& method)
    {
        _client.notify(method, std::string());

        _wsClient.process(CLIENT_PROCESS_TIMEOUT);
        for (size_t i = 0; i < SERVER_PROCESS_RETRIES; ++i)
            _brayns->render();
    }

    auto& getBrayns() { return *_brayns; }
    auto& getWsClient() { return _wsClient; }
    auto& getJsonRpcClient() { return _client; }
    void process()
    {
        _wsClient.process(CLIENT_PROCESS_TIMEOUT);
        _brayns->preRender();
    }

private:
    static ClientServer* _instance;
    std::unique_ptr<brayns::Brayns> _brayns;
    rockets::ws::Client _wsClient;
    rockets::jsonrpc::Client<rockets::ws::Client> _client{_wsClient};
};

ClientServer* ClientServer::_instance{nullptr};

template <typename Params, typename RetVal>
RetVal makeRequest(const std::string& method, const Params& params)
{
    return ClientServer::instance().makeRequest<Params, RetVal>(method, params);
}

template <typename RetVal>
RetVal makeRequest(const std::string& method, const std::string& params)
{
    return ClientServer::instance().makeRequest<RetVal>(method, params);
}

template <typename RetVal>
RetVal makeRequest(const std::string& method)
{
    return ClientServer::instance().makeRequest<RetVal>(method);
}

template <typename RetVal>
RetVal makeRequestUpdate(const std::string& method, RetVal baseObject)
{
    return ClientServer::instance().makeRequestUpdate<RetVal>(method,
                                                              baseObject);
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

brayns::Scene& getScene()
{
    return ClientServer::instance().getBrayns().getEngine().getScene();
}

brayns::Renderer& getRenderer()
{
    return ClientServer::instance().getBrayns().getEngine().getRenderer();
}

auto& getWsClient()
{
    return ClientServer::instance().getWsClient();
}

auto& getJsonRpcClient()
{
    return ClientServer::instance().getJsonRpcClient();
}

void process()
{
    ClientServer::instance().process();
}

void connect(rockets::ws::Client& client)
{
    ClientServer::instance().connect(client);
}
