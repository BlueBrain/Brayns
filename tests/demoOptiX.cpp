/* Copyright (c) 2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: jonas.karlsson@epfl.ch
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

#include <brayns/common/camera/Camera.h>
#include <brayns/common/engine/Engine.h>
#include <brayns/common/renderer/FrameBuffer.h>
#include <brayns/common/scene/Model.h>
#include <brayns/common/scene/Scene.h>

#define BOOST_TEST_MODULE demoOptiX
#include <boost/test/unit_test.hpp>

#include "PDiffHelpers.h"

BOOST_AUTO_TEST_CASE(render_demo)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    const char* argv[] = {app, "demo", "--engine", "optix"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);

    const brayns::Vector3f rotCenter = {0.5f, 0.5f, 0.5f};

    auto& camera = brayns.getEngine().getCamera();
    const auto camPos = camera.getPosition();

    camera.setTarget(rotCenter);
    camera.setPosition(camPos - (rotCenter - camPos));

    brayns.getEngine().getScene().setMaterialsColorMap(
        brayns::MaterialsColorMap::gradient);

    brayns.commitAndRender();

    BOOST_CHECK(compareTestImage("testdemoOptiX.png",
                                 brayns.getEngine().getFrameBuffer()));
}
