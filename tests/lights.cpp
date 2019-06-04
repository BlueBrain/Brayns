/* Copyright (c) 2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#define BOOST_TEST_MODULE lights
#include <brayns/Brayns.h>

#include <brayns/common/light/Light.h>
#include <brayns/engine/Camera.h>
#include <brayns/engine/Engine.h>
#include <brayns/engine/FrameBuffer.h>
#include <brayns/engine/LightManager.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>

#include <boost/test/unit_test.hpp>

#include "PDiffHelpers.h"

namespace
{
const auto YELLOW = brayns::Vector3f(1.0f, 1.0f, 0.0f);
const auto BLUE = brayns::Vector3f(0.0f, 0.0f, 1.0f);

const float lampHeight = 0.99f;
const float lampWidth = 0.15f;

const brayns::Vector3f lampCentre = {0.5f, lampHeight, 0.5f};

const brayns::Vector3f lampPositions[4] = {
    {lampCentre.x - lampWidth, lampHeight, lampCentre.z - lampWidth},
    {lampCentre.x + lampWidth, lampHeight, lampCentre.z - lampWidth},
    {lampCentre.x + lampWidth, lampHeight, lampCentre.z + lampWidth},
    {lampCentre.x - lampWidth, lampHeight, lampCentre.z + lampWidth}};
} // namespace

BOOST_AUTO_TEST_CASE(render_scivis_quadlight)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    const char* argv[] = {app,          "demo",   "--engine",       "ospray",
                          "--renderer", "scivis", "--no-head-light"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);

    brayns.getEngine().getScene().getLightManager().addLight(
        std::make_shared<brayns::QuadLight>(
            lampPositions[0], (lampPositions[1] - lampPositions[0]),
            (lampPositions[3] - lampPositions[0]), YELLOW, 1.0f, true));

    brayns.commitAndRender();

    BOOST_CHECK(compareTestImage("testLightScivisQuadLight.png",
                                 brayns.getEngine().getFrameBuffer()));
}

BOOST_AUTO_TEST_CASE(render_scivis_spotlight)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    const char* argv[] = {app,          "demo",   "--engine",       "ospray",
                          "--renderer", "scivis", "--no-head-light"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);

    brayns.getEngine().getScene().getLightManager().addLight(
        std::make_shared<brayns::SpotLight>(lampCentre,
                                            brayns::Vector3f(0, -1, 0), 90.f,
                                            10.f, lampWidth, BLUE, 1.0f, true));
    brayns.commitAndRender();

    BOOST_CHECK(compareTestImage("testLightScivisSpotLight.png",
                                 brayns.getEngine().getFrameBuffer()));
}
