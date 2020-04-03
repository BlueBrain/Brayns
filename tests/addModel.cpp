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

#include <jsonSerialization.h>

#include <tests/paths.h>

#include "ClientServer.h"

#include <fstream>

const std::string ADD_MODEL("add-model");

TEST_CASE_FIXTURE(ClientServer, "missing_params")
{
    try
    {
        makeRequest<brayns::ModelParams, brayns::ModelDescriptor>(ADD_MODEL,
                                                                  {});
        REQUIRE(false);
    }
    catch (const rockets::jsonrpc::response_error& e)
    {
        CHECK_EQ(e.code, brayns::ERROR_ID_MISSING_PARAMS);
        CHECK(e.data.empty());
    }
}

TEST_CASE_FIXTURE(ClientServer, "nonexistant_file")
{
    try
    {
        makeRequest<brayns::ModelParams, brayns::ModelDescriptor>(
            ADD_MODEL, {"wrong", "wrong.xyz"});
        REQUIRE(false);
    }
    catch (const rockets::jsonrpc::response_error& e)
    {
        CHECK_EQ(e.code, brayns::ERROR_ID_LOADING_BINARY_FAILED);
        CHECK(e.data.empty());
    }
}

TEST_CASE_FIXTURE(ClientServer, "unsupported_type")
{
    try
    {
        makeRequest<brayns::ModelParams, brayns::ModelDescriptor>(
            ADD_MODEL, {"unsupported", BRAYNS_TESTDATA_MODEL_UNSUPPORTED_PATH});
        REQUIRE(false);
    }
    catch (const rockets::jsonrpc::response_error& e)
    {
        CHECK_EQ(e.code, brayns::ERROR_ID_UNSUPPORTED_TYPE);
        REQUIRE(e.data.empty());
    }
}

TEST_CASE_FIXTURE(ClientServer, "xyz")
{
    const auto numModels = getScene().getNumModels();
    CHECK_NOTHROW((makeRequest<brayns::ModelParams, brayns::ModelDescriptor>(
        ADD_MODEL, {"monkey", BRAYNS_TESTDATA_MODEL_MONKEY_PATH})));
    CHECK_EQ(getScene().getNumModels(), numModels + 1);
}

TEST_CASE_FIXTURE(ClientServer, "obj")
{
    const auto numModels = getScene().getNumModels();
    brayns::ModelParams params{"bennu", BRAYNS_TESTDATA_MODEL_BENNU_PATH};
    params.setVisible(false);
    auto model =
        makeRequest<brayns::ModelParams, brayns::ModelDescriptor>(ADD_MODEL,
                                                                  params);
    CHECK_EQ(getScene().getNumModels(), numModels + 1);
    CHECK_EQ(model.getName(), params.getName());
    CHECK_EQ(model.getPath(), params.getPath());
    CHECK(!model.getVisible());
}

TEST_CASE_FIXTURE(ClientServer, "xyz_obj")
{
    const auto initialNbModels = getScene().getNumModels();
    CHECK_NOTHROW((makeRequest<brayns::ModelParams, brayns::ModelDescriptor>(
        ADD_MODEL, {"monkey", BRAYNS_TESTDATA_MODEL_MONKEY_PATH})));
    CHECK_NOTHROW((makeRequest<brayns::ModelParams, brayns::ModelDescriptor>(
        ADD_MODEL, {"bennu", BRAYNS_TESTDATA_MODEL_BENNU_PATH})));
    auto newNbModels = getScene().getNumModels();
    CHECK_EQ(initialNbModels + 2, newNbModels);
    getScene().removeModel(newNbModels - 1);
    newNbModels = getScene().getNumModels();
    CHECK_EQ(initialNbModels + 1, newNbModels);
}

TEST_CASE_FIXTURE(ClientServer, "broken_xyz")
{
    try
    {
        makeRequest<brayns::ModelParams, brayns::ModelDescriptor>(
            ADD_MODEL, {"broken", BRAYNS_TESTDATA_MODEL_BROKEN_PATH});
        REQUIRE(false);
    }
    catch (const rockets::jsonrpc::response_error& e)
    {
        CHECK_EQ(e.code, brayns::ERROR_ID_LOADING_BINARY_FAILED);
        CHECK(std::string(e.what()) ==
              "Invalid content in line 1: 2.500000 3.437500");
    }
}

#if BRAYNS_USE_LIBARCHIVE
TEST_CASE_FIXTURE(ClientServer, "obj_zip")
{
    const auto numModels = getScene().getNumModels();
    brayns::ModelParams params{"bennu", BRAYNS_TESTDATA_MODEL_BENNU_PATH};
    auto model =
        makeRequest<brayns::ModelParams, brayns::ModelDescriptor>(ADD_MODEL,
                                                                  params);
    CHECK_EQ(getScene().getNumModels(), numModels + 1);
    CHECK_EQ(model.getName(), params.getName());
    CHECK_EQ(model.getPath(), params.getPath());
}
#endif

TEST_CASE_FIXTURE(ClientServer, "mesh_loader_properties_valid")
{
    const auto numModels = getScene().getNumModels();
    brayns::PropertyMap properties;
    properties.setProperty({"geometryQuality", std::string("low")});
    properties.setProperty({"unused", 42});
    brayns::ModelParams params{"bennu", BRAYNS_TESTDATA_MODEL_BENNU_PATH,
                               properties};

    auto model =
        makeRequest<brayns::ModelParams, brayns::ModelDescriptor>(ADD_MODEL,
                                                                  params);
    CHECK_EQ(getScene().getNumModels(), numModels + 1);
    CHECK_EQ(model.getName(), params.getName());
    CHECK_EQ(model.getPath(), params.getPath());
}

TEST_CASE_FIXTURE(ClientServer, "mesh_loader_properties_invalid")
{
    brayns::PropertyMap properties;
    properties.setProperty({"geometryQuality", std::string("INVALID")});
    brayns::ModelParams params{"bennu", BRAYNS_TESTDATA_MODEL_BENNU_PATH,
                               properties};

    try
    {
        makeRequest<brayns::ModelParams, brayns::ModelDescriptor>(ADD_MODEL,
                                                                  params);
        REQUIRE(false);
    }
    catch (std::runtime_error& e)
    {
        CHECK(std::string(e.what()) == "Could not match enum 'INVALID'");
    }
}

TEST_CASE_FIXTURE(ClientServer, "protein_loader")
{
    brayns::PropertyMap properties;
    properties.setProperty({"radiusMultiplier", 2.5});
    properties.setProperty(
        {"colorScheme",
         brayns::enumToString(brayns::ProteinColorScheme::protein_chains),
         brayns::enumNames<brayns::ProteinColorScheme>(),
         {}});

    const auto numModels = getScene().getNumModels();

    brayns::ModelParams params{"1mbs", BRAYNS_TESTDATA_MODEL_PDB_PATH,
                               properties};

    auto model =
        makeRequest<brayns::ModelParams, brayns::ModelDescriptor>(ADD_MODEL,
                                                                  params);
    CHECK_EQ(getScene().getNumModels(), numModels + 1);
    CHECK_EQ(model.getName(), params.getName());
    CHECK_EQ(model.getPath(), params.getPath());
    CHECK(!model.getBounds().isEmpty());
}

TEST_CASE_FIXTURE(ClientServer, "cancel")
{
    auto request = getJsonRpcClient()
                       .request<brayns::ModelParams, brayns::ModelDescriptor>(
                           ADD_MODEL, {"forever", "forever"});

    request.cancel();

    while (!request.is_ready())
        process();

    CHECK_THROWS_AS(request.get(), std::runtime_error);
}

TEST_CASE_FIXTURE(ClientServer, "close_client_while_pending_request")
{
    auto wsClient = std::make_unique<rockets::ws::Client>();

    connect(*wsClient);

    auto responseFuture =
        rockets::jsonrpc::Client<rockets::ws::Client>{*wsClient}
            .request<brayns::ModelParams, brayns::ModelDescriptor>(
                ADD_MODEL, {"monkey", BRAYNS_TESTDATA_MODEL_MONKEY_PATH});

    auto asyncWait =
        std::async(std::launch::async, [&responseFuture, &wsClient, this] {
            wsClient->process(10);
            process();

            wsClient.reset(); // close client connection
            process();

            responseFuture.get();
        });

    CHECK_THROWS_AS(asyncWait.get(), rockets::jsonrpc::response_error);
}

TEST_CASE_FIXTURE(ClientServer, "folder")
{
    CHECK_THROWS_AS(
        (makeRequest<brayns::ModelParams, brayns::ModelDescriptorPtr>(
            ADD_MODEL, {"folder", BRAYNS_TESTDATA_VALID_MODELS_PATH})),
        rockets::jsonrpc::response_error);
}
