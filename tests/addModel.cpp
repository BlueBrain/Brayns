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

#define BOOST_TEST_MODULE braynsAddModel

#include <jsonSerialization.h>

#include <tests/paths.h>

#include "ClientServer.h"

#include <fstream>

const std::string ADD_MODEL("add-model");

BOOST_GLOBAL_FIXTURE(ClientServer);

BOOST_AUTO_TEST_CASE(missing_params)
{
    try
    {
        makeRequest<brayns::ModelParams, brayns::ModelDescriptor>(ADD_MODEL,
                                                                  {});
        BOOST_REQUIRE(false);
    }
    catch (const rockets::jsonrpc::response_error& e)
    {
        BOOST_CHECK_EQUAL(e.code, brayns::ERROR_ID_MISSING_PARAMS);
        BOOST_CHECK(e.data.empty());
    }
}

BOOST_AUTO_TEST_CASE(nonexistant_file)
{
    try
    {
        makeRequest<brayns::ModelParams, brayns::ModelDescriptor>(
            ADD_MODEL, {"wrong", "wrong.xyz"});
        BOOST_REQUIRE(false);
    }
    catch (const rockets::jsonrpc::response_error& e)
    {
        BOOST_CHECK_EQUAL(e.code, brayns::ERROR_ID_LOADING_BINARY_FAILED);
        BOOST_CHECK(e.data.empty());
    }
}

BOOST_AUTO_TEST_CASE(unsupported_type)
{
    try
    {
        makeRequest<brayns::ModelParams, brayns::ModelDescriptor>(
            ADD_MODEL, {"unsupported", BRAYNS_TESTDATA_MODEL_UNSUPPORTED_PATH});
        BOOST_REQUIRE(false);
    }
    catch (const rockets::jsonrpc::response_error& e)
    {
        BOOST_CHECK_EQUAL(e.code, brayns::ERROR_ID_UNSUPPORTED_TYPE);
        BOOST_REQUIRE(e.data.empty());
    }
}

BOOST_AUTO_TEST_CASE(xyz)
{
    const auto numModels = getScene().getNumModels();
    BOOST_CHECK_NO_THROW(
        (makeRequest<brayns::ModelParams, brayns::ModelDescriptor>(
            ADD_MODEL, {"monkey", BRAYNS_TESTDATA_MODEL_MONKEY_PATH})));
    BOOST_CHECK_EQUAL(getScene().getNumModels(), numModels + 1);
}

BOOST_AUTO_TEST_CASE(obj)
{
    const auto numModels = getScene().getNumModels();
    brayns::ModelParams params{"bennu", BRAYNS_TESTDATA_MODEL_BENNU_PATH};
    params.setVisible(false);
    auto model =
        makeRequest<brayns::ModelParams, brayns::ModelDescriptor>(ADD_MODEL,
                                                                  params);
    BOOST_CHECK_EQUAL(getScene().getNumModels(), numModels + 1);
    BOOST_CHECK_EQUAL(model.getName(), params.getName());
    BOOST_CHECK_EQUAL(model.getPath(), params.getPath());
    BOOST_CHECK(!model.getVisible());
}

BOOST_AUTO_TEST_CASE(xyz_obj)
{
    const auto initialNbModels = getScene().getNumModels();
    BOOST_CHECK_NO_THROW(
        (makeRequest<brayns::ModelParams, brayns::ModelDescriptor>(
            ADD_MODEL, {"monkey", BRAYNS_TESTDATA_MODEL_MONKEY_PATH})));
    BOOST_CHECK_NO_THROW(
        (makeRequest<brayns::ModelParams, brayns::ModelDescriptor>(
            ADD_MODEL, {"bennu", BRAYNS_TESTDATA_MODEL_BENNU_PATH})));
    auto newNbModels = getScene().getNumModels();
    BOOST_CHECK_EQUAL(initialNbModels + 2, newNbModels);
    getScene().removeModel(newNbModels - 1);
    getScene().markModified();
    newNbModels = getScene().getNumModels();
    BOOST_CHECK_EQUAL(initialNbModels + 1, newNbModels);
}

BOOST_AUTO_TEST_CASE(broken_xyz)
{
    try
    {
        makeRequest<brayns::ModelParams, brayns::ModelDescriptor>(
            ADD_MODEL, {"broken", BRAYNS_TESTDATA_MODEL_BROKEN_PATH});
        BOOST_REQUIRE(false);
    }
    catch (const rockets::jsonrpc::response_error& e)
    {
        BOOST_CHECK_EQUAL(e.code, brayns::ERROR_ID_LOADING_BINARY_FAILED);
        BOOST_CHECK_EQUAL(e.what(),
                          "Invalid content in line 1: 2.500000 3.437500");
    }
}

BOOST_AUTO_TEST_CASE(cancel)
{
    auto request = getJsonRpcClient()
                       .request<brayns::ModelParams, brayns::ModelDescriptor>(
                           ADD_MODEL, {"forever", "forever"});

    request.cancel();

    while (!request.is_ready())
        process();

    BOOST_CHECK_THROW(request.get(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(close_client_while_pending_request)
{
    auto wsClient = std::make_unique<rockets::ws::Client>();

    connect(*wsClient);

    auto responseFuture =
        rockets::jsonrpc::Client<rockets::ws::Client>{*wsClient}
            .request<brayns::ModelParams, brayns::ModelDescriptor>(
                ADD_MODEL, {"monkey", BRAYNS_TESTDATA_MODEL_MONKEY_PATH});

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
    BOOST_CHECK_THROW(
        (makeRequest<brayns::ModelParams, brayns::ModelDescriptorPtr>(
            ADD_MODEL, {"folder", BRAYNS_TESTDATA_VALID_MODELS_PATH})),
        rockets::jsonrpc::response_error);
}
