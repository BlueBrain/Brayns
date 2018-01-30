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

#include <brayns/Brayns.h>
#include <brayns/common/engine/Engine.h>
#include <brayns/parameters/ParametersManager.h>

#include <rockets/jsonrpc/client.h>
#include <rockets/ws/client.h>

#define BOOST_TEST_MODULE braynsWebAPI
#include <boost/test/unit_test.hpp>

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#endif
#include <plugins/extensions/plugins/rapidjson/document.h>
#include <plugins/extensions/plugins/rapidjson/prettywriter.h>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

template <typename T>
bool is_ready(const std::future<T>& f)
{
    return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

std::string buildRequest(const std::string& method, const size_t id,
                         const std::string& params)
{
    using namespace rapidjson;
    Document object(kObjectType);

    object.AddMember(StringRef("jsonrpc"), StringRef("2.0"),
                     object.GetAllocator());
    object.AddMember(StringRef("method"), StringRef(method.c_str()),
                     object.GetAllocator());
    object.AddMember(StringRef("id"), id, object.GetAllocator());

    Document param(kObjectType);
    param.Parse(params.c_str());
    object.AddMember(StringRef("params"), param, object.GetAllocator());

    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    object.Accept(writer);
    return buffer.GetString();
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

    rapidjson::Document makeRequest(const std::string& method,
                                    const std::string& params = std::string())
    {
        auto responseFuture = client.request(method, params);
        while (!is_ready(responseFuture))
        {
            brayns->render();
            wsClient.process(0);
        }

        using namespace rapidjson;
        Document object(kObjectType);
        auto response = responseFuture.get();
        object.Parse(response.result.c_str());
        return object;
    }

private:
    static ClientServer* _instance;
    std::unique_ptr<brayns::Brayns> brayns;
    rockets::ws::Client wsClient;
    rockets::jsonrpc::Client<rockets::ws::Client> client{wsClient};
};

ClientServer* ClientServer::_instance{nullptr};

rapidjson::Document makeRequest(const std::string& method,
                                const std::string& params = std::string())
{
    return ClientServer::instance().makeRequest(method, params);
}

std::ostream& operator<<(std::ostream& os, const rapidjson::Value& value)
{
    using namespace rapidjson;
    StringBuffer sb;
    PrettyWriter<StringBuffer> writer(sb);
    value.Accept(writer);
    os << sb.GetString() << std::endl;
    return os;
}

rapidjson::Document to_json(const std::string& data)
{
    using namespace rapidjson;
    Document object(kObjectType);
    object.Parse(data.c_str());
    return object;
}

BOOST_GLOBAL_FIXTURE(ClientServer);

BOOST_AUTO_TEST_CASE(reset_camera)
{
    BOOST_CHECK(makeRequest("reset-camera") == to_json("\"OK\""));
}

BOOST_AUTO_TEST_CASE(inspect)
{
    auto inspectResult = makeRequest("inspect", "[0.5,0.5]");
    BOOST_CHECK(inspectResult["hit"].GetBool());
    auto jsonArray = inspectResult["position"].GetArray();
    const brayns::Vector3f position{jsonArray[0].GetFloat(),
                                    jsonArray[1].GetFloat(),
                                    jsonArray[2].GetFloat()};
    BOOST_CHECK(position.equals(
        {0.500001490116119, 0.500001490116119, 1.19209289550781e-7}));

    auto failedInspectResult = makeRequest("inspect", "[10,-10]");
    BOOST_CHECK(!failedInspectResult["hit"].GetBool());
}
