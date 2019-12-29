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

#include <jsonPropertyMap.h>

#include "ClientServer.h"
#include <brayns/engineapi/Renderer.h>

TEST_CASE_FIXTURE(ClientServer, "change_fov")
{
    brayns::PropertyMap cameraParams;
    cameraParams.setProperty({"fovy", 10., .1, 360.});
    CHECK((makeRequest<brayns::PropertyMap, bool>("set-camera-params",
                                                  cameraParams)));
}

TEST_CASE_FIXTURE(ClientServer, "reset_camera")
{
    const auto& orientation = getCamera().getOrientation();
    getCamera().setOrientation({0, 0, 0, 1});
    makeNotification("reset-camera");
    CHECK_EQ(getCamera().getOrientation(), orientation);
}

TEST_CASE_FIXTURE(ClientServer, "inspect")
{
    auto inspectResult =
        makeRequest<std::array<double, 2>, brayns::Renderer::PickResult>(
            "inspect", {{0.5, 0.5}});
    CHECK(inspectResult.hit);
    CHECK(
        glm::all(glm::epsilonEqual(inspectResult.pos,
                                   {0.5, 0.5, 1.19209289550781e-7}, 0.000001)));

    auto failedInspectResult =
        makeRequest<std::array<double, 2>, brayns::Renderer::PickResult>(
            "inspect", {{10, -10}});
    CHECK(!failedInspectResult.hit);
}

TEST_CASE_FIXTURE(ClientServer, "schema_non_existing_endpoint")
{
    CHECK_THROWS_AS((makeRequest<brayns::SchemaParam, std::string>("schema",
                                                                   {"foo"})),
                    std::runtime_error);
}

TEST_CASE_FIXTURE(ClientServer, "schema")
{
    std::string result = makeRequestJSONReturn<brayns::SchemaParam>(
        "schema", brayns::SchemaParam{"camera"});

    using namespace rapidjson;
    Document json(kObjectType);
    json.Parse(result.c_str());
    CHECK(json.HasMember("title"));
}
