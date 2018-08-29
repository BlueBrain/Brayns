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

#define BOOST_TEST_MODULE braynsAddModelFromBlob

#include <jsonSerialization.h>

#include <tests/paths.h>

#include "ClientServer.h"

#include <fstream>

const std::string REQUEST_MODEL_UPLOAD("request-model-upload");
const std::string CHUNK("chunk");

BOOST_GLOBAL_FIXTURE(ClientServer);

BOOST_AUTO_TEST_CASE(illegal_no_request)
{
    const std::string illegal("illegal");
    getWsClient().sendBinary(illegal.data(), illegal.size());
    process();
    // nothing to test, Brayns ignores the message and prints a warning
}

BOOST_AUTO_TEST_CASE(illegal_no_params)
{
    try
    {
        makeRequest<brayns::BinaryParam, brayns::ModelDescriptor>(
            REQUEST_MODEL_UPLOAD, {});
        BOOST_REQUIRE(false);
    }
    catch (const rockets::jsonrpc::response_error& e)
    {
        BOOST_CHECK_EQUAL(e.code, -1731);
        BOOST_CHECK(e.data.empty());
    }
}

BOOST_AUTO_TEST_CASE(missing_params)
{
    brayns::BinaryParam params;
    try
    {
        makeRequest<brayns::BinaryParam, brayns::ModelDescriptor>(
            REQUEST_MODEL_UPLOAD, {params});
        BOOST_REQUIRE(false);
    }
    catch (const rockets::jsonrpc::response_error& e)
    {
        BOOST_CHECK_EQUAL(e.code, -1731);
        BOOST_CHECK(e.data.empty());
    }
}

BOOST_AUTO_TEST_CASE(invalid_size)
{
    brayns::BinaryParam params;
    params.type = "xyz";
    params.size = 0;
    try
    {
        makeRequest<brayns::BinaryParam, brayns::ModelDescriptor>(
            REQUEST_MODEL_UPLOAD, {params});
        BOOST_REQUIRE(false);
    }
    catch (const rockets::jsonrpc::response_error& e)
    {
        BOOST_CHECK_EQUAL(e.code, -1731);
        BOOST_CHECK(e.data.empty());
    }
}

BOOST_AUTO_TEST_CASE(unsupported_type)
{
    brayns::BinaryParam params;
    params.type = "blub";
    params.size = 4;
    try
    {
        makeRequest<brayns::BinaryParam, brayns::ModelDescriptor>(
            REQUEST_MODEL_UPLOAD, {params});
        BOOST_REQUIRE(false);
    }
    catch (const rockets::jsonrpc::response_error& e)
    {
        BOOST_CHECK_EQUAL(e.code, -1732);
        BOOST_REQUIRE(!e.data.empty());
        brayns::BinaryError error;
        BOOST_CHECK(from_json(error, e.data));
        BOOST_CHECK_GT(error.supportedTypes.size(), 0);
    }
}

BOOST_AUTO_TEST_CASE(xyz)
{
    brayns::BinaryParam params;
    params.size = [] {
        std::ifstream file(BRAYNS_TESTDATA_MODEL_MONKEY_PATH,
                           std::ios::binary | std::ios::ate);
        return file.tellg();
    }();
    params.type = "xyz";
    params.setPath("monkey.xyz");

    auto request = getJsonRpcClient()
                       .request<brayns::BinaryParam, brayns::ModelDescriptor>(
                           REQUEST_MODEL_UPLOAD, {params});

    std::ifstream file(BRAYNS_TESTDATA_MODEL_MONKEY_PATH, std::ios::binary);

    std::vector<char> buffer(1024, 0);

    while (file.read(buffer.data(), buffer.size()))
    {
        const std::streamsize size = file.gcount();
        getWsClient().sendBinary(buffer.data(), size);
        process();
    }

    // read & send last chunk
    const std::streamsize size = file.gcount();
    if (size != 0)
    {
        file.read(buffer.data(), size);
        getWsClient().sendBinary(buffer.data(), size);
    }

    while (!request.is_ready())
        process();
    const auto& model = request.get();
    BOOST_CHECK_EQUAL(model.getName(), "monkey");
    BOOST_CHECK_EQUAL(model.getPath(), "monkey.xyz");
}

BOOST_AUTO_TEST_CASE(broken_xyz)
{
    brayns::BinaryParam params;
    params.size = [] {
        std::ifstream file(BRAYNS_TESTDATA_MODEL_BROKEN_PATH,
                           std::ios::binary | std::ios::ate);
        return file.tellg();
    }();
    params.type = "xyz";

    auto request = getJsonRpcClient()
                       .request<brayns::BinaryParam, brayns::ModelDescriptor>(
                           REQUEST_MODEL_UPLOAD, {params});

    std::ifstream file(BRAYNS_TESTDATA_MODEL_BROKEN_PATH, std::ios::binary);

    std::vector<char> buffer(1024, 0);

    while (file.read(buffer.data(), buffer.size()))
    {
        const std::streamsize size = file.gcount();
        getWsClient().sendBinary(buffer.data(), size);
        process();
    }

    // read & send last chunk
    const std::streamsize size = file.gcount();
    if (size != 0)
    {
        file.read(buffer.data(), size);
        getWsClient().sendBinary(buffer.data(), size);
    }

    while (!request.is_ready())
        process();
    try
    {
        request.get();
    }
    catch (const rockets::jsonrpc::response_error& e)
    {
        BOOST_CHECK_EQUAL(e.code, -1734);
        BOOST_CHECK_EQUAL(e.what(),
                          "Invalid content in line 1: 2.500000 3.437500");
    }
}

BOOST_AUTO_TEST_CASE(cancel)
{
    brayns::BinaryParam params;
    params.size = 42;
    params.type = "xyz";

    auto request = getJsonRpcClient()
                       .request<brayns::BinaryParam, brayns::ModelDescriptor>(
                           REQUEST_MODEL_UPLOAD, {params});

    request.cancel();

    while (!request.is_ready())
        process();

    BOOST_CHECK_THROW(request.get(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(send_wrong_number_of_bytes)
{
    brayns::BinaryParam params;
    params.size = 4;
    params.type = "xyz";

    auto request = getJsonRpcClient()
                       .request<brayns::BinaryParam, brayns::ModelDescriptor>(
                           REQUEST_MODEL_UPLOAD, {params});

    const std::string wrong("not_four_bytes");
    getWsClient().sendBinary(wrong.data(), wrong.size());

    try
    {
        while (!request.is_ready())
            process();
        request.get();
        BOOST_REQUIRE(false);
    }
    catch (const rockets::jsonrpc::response_error& e)
    {
        BOOST_CHECK_EQUAL(e.code, -1733);
    }
}

BOOST_AUTO_TEST_CASE(cancel_while_loading)
{
    brayns::BinaryParam params;
    params.size = 4;
    params.type = "forever";

    auto request = getJsonRpcClient()
                       .request<brayns::BinaryParam, brayns::ModelDescriptor>(
                           REQUEST_MODEL_UPLOAD, {params});

    const std::string fourBytes("four");
    getWsClient().sendBinary(fourBytes.data(), fourBytes.size());

    process();

    request.cancel();

    while (!request.is_ready())
        process();

    BOOST_CHECK_THROW(request.get(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(close_client_while_pending_request)
{
    auto wsClient = std::make_unique<rockets::ws::Client>();

    connect(*wsClient);

    brayns::BinaryParam params;
    params.size = 4;
    params.type = "xyz";

    auto responseFuture =
        rockets::jsonrpc::Client<rockets::ws::Client>{*wsClient}
            .request<brayns::BinaryParam, brayns::ModelDescriptor>(
                REQUEST_MODEL_UPLOAD, {params});

    auto asyncWait =
        std::async(std::launch::async, [&responseFuture, &wsClient] {
            wsClient->process(10);
            process();

            wsClient.reset(); // close client connection
            process();

            responseFuture.get();
        });

    BOOST_CHECK_THROW(asyncWait.get(), rockets::jsonrpc::response_error);
}

BOOST_AUTO_TEST_CASE(obj)
{
    brayns::BinaryParam params;
    params.size = [] {
        std::ifstream file(BRAYNS_TESTDATA_MODEL_BENNU_PATH,
                           std::ios::binary | std::ios::ate);
        return file.tellg();
    }();
    params.type = "obj";
    params.setName("bennu");

    auto request = getJsonRpcClient()
                       .request<brayns::BinaryParam, brayns::ModelDescriptor>(
                           REQUEST_MODEL_UPLOAD, {params});

    std::ifstream file(BRAYNS_TESTDATA_MODEL_BENNU_PATH, std::ios::binary);

    std::vector<char> buffer(1024, 0);

    while (file.read(buffer.data(), buffer.size()))
    {
        const std::streamsize size = file.gcount();
        getWsClient().sendBinary(buffer.data(), size);
        process();
    }

    // read & send last chunk
    const std::streamsize size = file.gcount();
    if (size != 0)
    {
        file.read(buffer.data(), size);
        getWsClient().sendBinary(buffer.data(), size);
    }

    while (!request.is_ready())
        process();
    const auto& model = request.get();
    BOOST_CHECK_EQUAL(model.getName(), "bennu");
}

BOOST_AUTO_TEST_CASE(concurrent_requests)
{
    brayns::BinaryParam xyzParams;
    xyzParams.size = [] {
        std::ifstream file(BRAYNS_TESTDATA_MODEL_MONKEY_PATH,
                           std::ios::binary | std::ios::ate);
        return file.tellg();
    }();
    xyzParams.type = "xyz";
    xyzParams.chunksID = 1;
    xyzParams.setPath("monkey.xyz");

    auto xyzRequest =
        getJsonRpcClient()
            .request<brayns::BinaryParam, brayns::ModelDescriptor>(
                REQUEST_MODEL_UPLOAD, {xyzParams});

    std::ifstream xyzFile(BRAYNS_TESTDATA_MODEL_MONKEY_PATH, std::ios::binary);

    ///////////////////

    brayns::BinaryParam objParams;
    objParams.size = [] {
        std::ifstream file(BRAYNS_TESTDATA_MODEL_BENNU_PATH,
                           std::ios::binary | std::ios::ate);
        return file.tellg();
    }();
    objParams.type = "obj";
    objParams.chunksID = 2;
    objParams.setName("bennu");

    auto objRequest =
        getJsonRpcClient()
            .request<brayns::BinaryParam, brayns::ModelDescriptor>(
                REQUEST_MODEL_UPLOAD, {objParams});

    std::ifstream objFile(BRAYNS_TESTDATA_MODEL_BENNU_PATH, std::ios::binary);

    ///////////////////

    process();

    std::array<char, 1024> buffer;
    bool xyzDone = false;
    bool objDone = false;
    while (!xyzDone || !objDone)
    {
        if (!xyzDone)
        {
            if (xyzFile.read(buffer.data(), buffer.size()))
            {
                getJsonRpcClient().notify<brayns::Chunk>(CHUNK,
                                                         {xyzParams.chunksID});
                const std::streamsize size = xyzFile.gcount();
                getWsClient().sendBinary(buffer.data(), size);
            }
            else
            {
                // read & send last chunk
                const std::streamsize size = xyzFile.gcount();
                if (size != 0)
                {
                    getJsonRpcClient().notify<brayns::Chunk>(
                        CHUNK, {xyzParams.chunksID});
                    xyzFile.read(buffer.data(), size);
                    getWsClient().sendBinary(buffer.data(), size);
                }
                xyzDone = true;
            }
        }

        if (!objDone)
        {
            if (objFile.read(buffer.data(), buffer.size()))
            {
                getJsonRpcClient().notify<brayns::Chunk>(CHUNK,
                                                         {objParams.chunksID});
                const std::streamsize size = objFile.gcount();
                getWsClient().sendBinary(buffer.data(), size);
            }
            else
            {
                // read & send last chunk
                const std::streamsize size = objFile.gcount();
                if (size != 0)
                {
                    getJsonRpcClient().notify<brayns::Chunk>(
                        CHUNK, {objParams.chunksID});
                    objFile.read(buffer.data(), size);
                    getWsClient().sendBinary(buffer.data(), size);
                }
                objDone = true;
            }
        }
        process();
    }

    while (!xyzRequest.is_ready() || !objRequest.is_ready())
        process();

    const auto& xyzModel = xyzRequest.get();
    BOOST_CHECK_EQUAL(xyzModel.getName(), "monkey");
    BOOST_CHECK_EQUAL(xyzModel.getPath(), "monkey.xyz");

    const auto& objModel = objRequest.get();
    BOOST_CHECK_EQUAL(objModel.getName(), "bennu");
}
