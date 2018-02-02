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

#include "jsonSerialization.h"

#include <brayns/Brayns.h>
#include <brayns/common/engine/Engine.h>
#include <brayns/common/renderer/Renderer.h>
#include <brayns/parameters/ParametersManager.h>

#include <rockets/jsonrpc/client.h>
#include <rockets/ws/client.h>

#define BOOST_TEST_MODULE braynsWebAPI
#include <boost/test/unit_test.hpp>

#include "ImageGenerator.h"

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

    ClientServer()
    {
        auto& testSuite = boost::unit_test::framework::master_test_suite();
        const char* app = testSuite.argv[0];
        const char* argv[] = {app, "--http-server", ":0"};
        const int argc = sizeof(argv) / sizeof(char*);
        brayns.reset(new brayns::Brayns(argc, argv));
        brayns->render();

        const auto uri = brayns->getParametersManager()
                             .getApplicationParameters()
                             .getHttpServerURI();

        auto connectFuture = wsClient.connect("ws://" + uri, "rockets");
        while (!is_ready(connectFuture))
        {
            brayns->render();
            wsClient.process(5);
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
            brayns->render();
            wsClient.process(0);
        }

        return responseFuture.get();
    }

    bool makeRequest(const std::string& method)
    {
        auto responseFuture = client.request(method, {});
        while (!is_ready(responseFuture))
        {
            brayns->render();
            wsClient.process(0);
        }

        return responseFuture.get().result == "\"OK\"";
    }

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

bool makeRequest(const std::string& method)
{
    return ClientServer::instance().makeRequest(method);
}

BOOST_GLOBAL_FIXTURE(ClientServer);

BOOST_AUTO_TEST_CASE(reset_camera)
{
    BOOST_CHECK(makeRequest("reset-camera"));
}

BOOST_AUTO_TEST_CASE(inspect)
{
    auto inspectResult =
        makeRequest<std::array<float, 2>, brayns::Renderer::PickResult>(
            "inspect", {{0.5, 0.5}});
    BOOST_CHECK(inspectResult.hit);
    BOOST_CHECK(inspectResult.pos.equals(
        {0.500001490116119, 0.500001490116119, 1.19209289550781e-7}));

    auto failedInspectResult =
        makeRequest<std::array<float, 2>, brayns::Renderer::PickResult>(
            "inspect", {{10, -10}});
    BOOST_CHECK(!failedInspectResult.hit);
}

#ifdef BRAYNS_USE_MAGICKPP
BOOST_AUTO_TEST_CASE(snapshot)
{
    brayns::SnapshotParams params;
    params.format = "jpg";
    params.size = {5, 5};
    params.quality = 75;

    auto image =
        makeRequest<brayns::SnapshotParams,
                    brayns::ImageGenerator::ImageBase64>("snapshot", params);
    BOOST_CHECK_EQUAL(image.data,
                      "/9j/4AAQSkZJRgABAQAAAQABAAD/"
                      "2wBDAAgGBgcGBQgHBwcJCQgKDBQNDAsLDBkSEw8UHRofHh0aHBwgJC4n"
                      "ICIsIxwcKDcpLDAxNDQ0Hyc5PTgyPC4zNDL/"
                      "2wBDAQkJCQwLDBgNDRgyIRwhMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIy"
                      "MjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjL/"
                      "wAARCAAFAAUDASIAAhEBAxEB/8QAFQABAQAAAAAAAAAAAAAAAAAAAAX/"
                      "xAAgEAABAwMFAQAAAAAAAAAAAAACAAEEAwURBxIhMkGB/"
                      "8QAFQEBAQAAAAAAAAAAAAAAAAAABAb/"
                      "xAAcEQACAgIDAAAAAAAAAAAAAAABAgADBEEFEdH/2gAMAwEAAhEDEQA/"
                      "AJ0PVMbfBjwxsrmMekFJiKU3O0WHPT3GfqIir6OLxGqUlNDZ9hVsboT/"
                      "2Q==");
}
#endif

BOOST_AUTO_TEST_CASE(snapshot_empty_params)
{
    BOOST_CHECK_THROW((makeRequest<brayns::SnapshotParams,
                                   brayns::ImageGenerator::ImageBase64>(
                          "snapshot", brayns::SnapshotParams())),
                      rockets::jsonrpc::response_error);
}

BOOST_AUTO_TEST_CASE(snapshot_illegal_format)
{
    brayns::SnapshotParams params;
    params.size = {5, 5};
    params.format = "";
    BOOST_CHECK_THROW(
        (makeRequest<brayns::SnapshotParams,
                     brayns::ImageGenerator::ImageBase64>("snapshot", params)),
        rockets::jsonrpc::response_error);
}
