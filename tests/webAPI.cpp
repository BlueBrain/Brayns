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

#define BOOST_TEST_MODULE braynsWebAPI

#include <jsonPropertyMap.h>

#include "ClientServer.h"
#include <brayns/engine/Renderer.h>

BOOST_GLOBAL_FIXTURE(ClientServer);

BOOST_AUTO_TEST_CASE(change_fov)
{
    brayns::PropertyMap cameraParams;
    cameraParams.setProperty({"fovy", 10., .1, 360.});
    BOOST_CHECK((makeRequest<brayns::PropertyMap, bool>("set-camera-params",
                                                        cameraParams)));
}

BOOST_AUTO_TEST_CASE(reset_camera)
{
    const auto& orientation = getCamera().getOrientation();
    getCamera().setOrientation({0, 0, 0, 1});
    makeNotification("reset-camera");
    BOOST_CHECK_EQUAL(getCamera().getOrientation(), orientation);
}

BOOST_AUTO_TEST_CASE(inspect)
{
    auto inspectResult =
        makeRequest<std::array<double, 2>, brayns::Renderer::PickResult>(
            "inspect", {{0.5, 0.5}});
    BOOST_CHECK(inspectResult.hit);
    BOOST_CHECK(
        glm::all(glm::epsilonEqual(inspectResult.pos,
                                   {0.5, 0.5, 1.19209289550781e-7},
                                   0.000001)));

    auto failedInspectResult =
        makeRequest<std::array<double, 2>, brayns::Renderer::PickResult>(
            "inspect", {{10, -10}});
    BOOST_CHECK(!failedInspectResult.hit);
}

BOOST_AUTO_TEST_CASE(schema_non_existing_endpoint)
{
    BOOST_CHECK_THROW((makeRequest<brayns::SchemaParam, std::string>("schema",
                                                                     {"foo"})),
                      std::runtime_error);
}

BOOST_AUTO_TEST_CASE(schema)
{
    std::string result = makeRequestJSONReturn<brayns::SchemaParam>(
        "schema", brayns::SchemaParam{"camera"});

    using namespace rapidjson;
    Document json(kObjectType);
    json.Parse(result.c_str());
    BOOST_CHECK(json.HasMember("title"));
}
