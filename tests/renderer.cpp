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

const std::string GET_RENDERER("get-renderer");
const std::string SET_RENDERER("set-renderer");

const std::string GET_RENDERER_PARAMS("get-renderer-params");
const std::string SET_RENDERER_PARAMS("set-renderer-params");

TEST_CASE_FIXTURE(ClientServer, "get_renderer")
{
    auto renderer = makeRequest<brayns::RenderingParameters>(GET_RENDERER);
    CHECK_EQ(renderer.getCurrentRenderer(), "basic");
}

TEST_CASE_FIXTURE(ClientServer, "get_renderer_params")
{
    CHECK_EQ(getRenderer().getCurrentType(), "basic");
    auto rendererParams = makeRequest<brayns::PropertyMap>(GET_RENDERER_PARAMS);
    CHECK(rendererParams.getProperties().empty());
}

TEST_CASE_FIXTURE(ClientServer, "change_renderer_from_web_api")
{
    auto& renderer = getRenderer();
    CHECK_EQ(renderer.getCurrentType(), "basic");

    auto params = ClientServer::instance()
                      .getBrayns()
                      .getParametersManager()
                      .getRenderingParameters();
    params.setCurrentRenderer("scivis");
    CHECK(
        (makeRequest<brayns::RenderingParameters, bool>(SET_RENDERER, params)));
    CHECK_EQ(renderer.getCurrentType(), "scivis");

    brayns::PropertyMap scivisProps = renderer.getPropertyMap();
    auto rendererParams =
        makeRequestUpdate<brayns::PropertyMap>(GET_RENDERER_PARAMS,
                                               scivisProps);
    CHECK(!rendererParams.getProperties().empty());
    CHECK_EQ(rendererParams.getProperty<int>("aoSamples"), 1);

    rendererParams.updateProperty("aoSamples", 42);
    CHECK((makeRequest<brayns::PropertyMap, bool>(SET_RENDERER_PARAMS,
                                                  rendererParams)));
    CHECK_EQ(renderer.getPropertyMap().getProperty<int>("aoSamples"), 42);

    params.setCurrentRenderer("wrong");
    CHECK_THROWS_AS(
        (makeRequest<brayns::RenderingParameters, bool>(SET_RENDERER, params)),
        std::runtime_error);
    CHECK_EQ(renderer.getCurrentType(), "scivis");
}

TEST_CASE_FIXTURE(ClientServer, "update_current_renderer_property")
{
    auto& renderer = getRenderer();
    renderer.setCurrentType("scivis");

    CHECK(renderer.hasProperty("aoWeight"));
    CHECK_EQ(renderer.getProperty<double>("aoWeight"), 0.);

    renderer.updateProperty("aoWeight", 1.5);
    CHECK_EQ(renderer.getProperty<double>("aoWeight"), 1.5);
}
