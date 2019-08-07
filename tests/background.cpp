/* Copyright (c) 2019, EPFL/Blue Brain Project
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

#include "ClientServer.h"
#include "PDiffHelpers.h"
#include <tests/paths.h>

const std::string SET_ENV_MAP("set-environment-map");

TEST_CASE_FIXTURE(ClientServer, "set_environment_map")
{
    CHECK((makeRequest<brayns::EnvironmentMapParam, bool>(
        SET_ENV_MAP, {BRAYNS_TESTDATA_PATH "envmap.jpg"})));

    CHECK(getScene().hasEnvironmentMap());
    getCamera().setPosition({0, 0, 5});
    commitAndRender();
    pdiff::PerceptualDiffParameters parameters;
    parameters.luminance_only = true;
    CHECK(compareTestImage("envmap.png", getFrameBuffer(), parameters));
}

TEST_CASE_FIXTURE(ClientServer, "unset_environment_map")
{
    CHECK((makeRequest<brayns::EnvironmentMapParam, bool>(SET_ENV_MAP, {""})));

    CHECK(!getScene().hasEnvironmentMap());
    getCamera().setPosition({0, 0, 5});
    commitAndRender();
    CHECK(compareTestImage("no_envmap.png", getFrameBuffer()));
}

TEST_CASE_FIXTURE(ClientServer, "set_invalid_environment_map")
{
    CHECK(
        !(makeRequest<brayns::EnvironmentMapParam, bool>(SET_ENV_MAP,
                                                         {"dont_exists.jpg"})));

    CHECK(!getScene().hasEnvironmentMap());
}
