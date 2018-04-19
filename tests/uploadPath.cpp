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

#define BOOST_TEST_MODULE braynsUploadPath

#include <jsonSerialization.h>

#include <tests/paths.h>

#include "ClientServer.h"

#include <fstream>

const std::string UPLOAD_PATH("upload-path");

BOOST_GLOBAL_FIXTURE(ClientServer);

BOOST_AUTO_TEST_CASE(illegal_no_params)
{
    try
    {
        makeRequest<std::vector<brayns::BinaryParam>, bool>(UPLOAD_PATH, {});
    }
    catch (const rockets::jsonrpc::response_error& e)
    {
        BOOST_CHECK_EQUAL(e.code, -1731);
        BOOST_CHECK(e.data.empty());
    }
}

BOOST_AUTO_TEST_CASE(missing_params)
{
    try
    {
        makeRequest<std::vector<std::string>, bool>(UPLOAD_PATH, {});
    }
    catch (const rockets::jsonrpc::response_error& e)
    {
        BOOST_CHECK_EQUAL(e.code, -1731);
        BOOST_CHECK(e.data.empty());
    }
}

BOOST_AUTO_TEST_CASE(nonexistant_file)
{
    try
    {
        makeRequest<std::vector<std::string>, bool>(UPLOAD_PATH, {"boo!"});
    }
    catch (const rockets::jsonrpc::response_error& e)
    {
        BOOST_CHECK_EQUAL(e.code, -1735);
        BOOST_CHECK(e.data.empty());
    }
}

BOOST_AUTO_TEST_CASE(unsupported_type)
{
    try
    {
        makeRequest<std::vector<std::string>, bool>(
            UPLOAD_PATH, {BRAYNS_TESTDATA + std::string("unsupported.abc")});
    }
    catch (const rockets::jsonrpc::response_error& e)
    {
        BOOST_CHECK_EQUAL(e.code, -1732);
        BOOST_REQUIRE(!e.data.empty());
        brayns::BinaryError error;
        BOOST_CHECK(from_json(error, e.data));
        BOOST_CHECK_EQUAL(error.index, 0);
        BOOST_CHECK_GT(error.supportedTypes.size(), 0);
    }
}

BOOST_AUTO_TEST_CASE(multiple_files_one_unsupported)
{
    try
    {
        makeRequest<std::vector<std::string>, bool>(
            UPLOAD_PATH, {BRAYNS_TESTDATA + std::string("monkey.xyz"),
                          BRAYNS_TESTDATA + std::string("unsupported.abc")});
    }
    catch (const rockets::jsonrpc::response_error& e)
    {
        BOOST_REQUIRE(!e.data.empty());
        brayns::BinaryError error;
        BOOST_CHECK(from_json(error, e.data));
        BOOST_CHECK_EQUAL(error.index, 1); // fails on the first wrong param
        BOOST_CHECK_GT(error.supportedTypes.size(), 0);
    }
}

BOOST_AUTO_TEST_CASE(xyz)
{
    BOOST_CHECK((makeRequest<std::vector<std::string>, bool>(
        UPLOAD_PATH, {BRAYNS_TESTDATA + std::string("monkey.xyz")})));
}

BOOST_AUTO_TEST_CASE(obj)
{
    BOOST_CHECK((makeRequest<std::vector<std::string>, bool>(
        UPLOAD_PATH, {BRAYNS_TESTDATA + std::string("bennu.obj")})));
}

BOOST_AUTO_TEST_CASE(multiple_files)
{
    BOOST_CHECK((makeRequest<std::vector<std::string>, bool>(
        UPLOAD_PATH, {BRAYNS_TESTDATA + std::string("monkey.xyz"),
                      BRAYNS_TESTDATA + std::string("bennu.obj")})));
}

BOOST_AUTO_TEST_CASE(broken_xyz)
{
    try
    {
        makeRequest<std::vector<std::string>, bool>(
            UPLOAD_PATH, {BRAYNS_TESTDATA + std::string("broken.xyz")});
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
    auto request =
        getJsonRpcClient().request<std::vector<std::string>, bool>(UPLOAD_PATH,
                                                                   {"forever"});

    auto asyncWait = std::async(std::launch::async, [&request] {
        while (!request.is_ready())
            process();
        request.get();
    });

    request.cancel();

    BOOST_CHECK_THROW(asyncWait.get(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(close_client_while_pending_request)
{
    auto wsClient = std::make_unique<rockets::ws::Client>();

    connect(*wsClient);

    auto responseFuture =
        rockets::jsonrpc::Client<rockets::ws::Client>{*wsClient}
            .request<std::vector<std::string>, bool>(
                UPLOAD_PATH, {BRAYNS_TESTDATA + std::string("monkey.xyz")});

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

BOOST_AUTO_TEST_CASE(folder)
{
    try
    {
        makeRequest<std::vector<std::string>, bool>(UPLOAD_PATH,
                                                    {BRAYNS_TESTDATA});
    }
    catch (const rockets::jsonrpc::response_error& e)
    {
        BOOST_CHECK_EQUAL(e.code, -1736);
        BOOST_CHECK(e.data.empty());
    }
}

#ifdef BRAYNS_USE_BBPTESTDATA
BOOST_AUTO_TEST_CASE(file_no_extension)
{
    try
    {
        makeRequest<std::vector<std::string>, bool>(UPLOAD_PATH,
                                                    {BBP_TEST_BLUECONFIG3});
    }
    catch (const rockets::jsonrpc::response_error& e)
    {
        BOOST_CHECK_EQUAL(e.code, -1732);
        BOOST_REQUIRE(!e.data.empty());
        brayns::BinaryError error;
        BOOST_CHECK(from_json(error, e.data));
        BOOST_CHECK_EQUAL(error.index, 0);
        BOOST_CHECK_GT(error.supportedTypes.size(), 0);
    }
}
#endif
