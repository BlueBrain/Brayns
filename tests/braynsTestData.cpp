
/* Copyright (c) 2016, EPFL/Blue Brain Project
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

#include <brayns/Brayns.h>
#include <tests/paths.h>

#include <brayns/common/camera/Camera.h>
#include <brayns/common/engine/Engine.h>
#include <brayns/common/renderer/FrameBuffer.h>
#include <brayns/common/scene/Model.h>
#include <brayns/common/scene/Scene.h>

#define BOOST_TEST_MODULE braynsTestData
#include <boost/test/unit_test.hpp>

#include "PDiffHelpers.h"

BOOST_AUTO_TEST_CASE(render_circuit_and_compare)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    const char* argv[] = {app,
                          BBP_TEST_BLUECONFIG3,
                          "--disable-accumulation",
                          "--circuit-targets",
                          "Layer1",
                          "--samples-per-pixel",
                          "16"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);
    brayns.commitAndRender();
    BOOST_CHECK(compareTestImage("testdataLayer1.png",
                                 brayns.getEngine().getFrameBuffer()));
}

BOOST_AUTO_TEST_CASE(render_circuit_mtypes_and_compare)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    const char* argv[] = {app,
                          BBP_TEST_BLUECONFIG3,
                          "--disable-accumulation",
                          "--circuit-targets",
                          "MiniColumn_0",
                          "--samples-per-pixel",
                          "16",
                          "--color-scheme",
                          "neuron-by-mtype"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);
    brayns.getEngine().getScene().setMaterialsColorMap(
        brayns::MaterialsColorMap::gradient);
    brayns.commitAndRender();
    BOOST_CHECK(compareTestImage("testdataMiniColumn0_mtypes.png",
                                 brayns.getEngine().getFrameBuffer()));
}

BOOST_AUTO_TEST_CASE(render_circuit_with_color_and_compare)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const auto transfer_file = BRAYNS_TESTDATA_PATH "rat0.1dt";

    const char* app = testSuite.argv[0];
    const char* argv[] = {app,
                          "--circuit-config",
                          BBP_TEST_BLUECONFIG3,
                          "--circuit-targets",
                          "allmini50",
                          "--circuit-report",
                          "voltages",
                          "--renderer",
                          "advanced_simulation",
                          "--samples-per-pixel",
                          "16",
                          "--color-map-file",
                          transfer_file,
                          "--color-map-range",
                          "-66",
                          "-62",
                          "--animation-frame",
                          "50",
                          "--synchronous-mode"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);

    const auto rotCenter = brayns.getEngine()
                               .getScene()
                               .getModel(0)
                               ->getTransformation()
                               .getRotationCenter();

    auto& camera = brayns.getEngine().getCamera();
    const auto camPos = camera.getPosition();

    camera.setTarget(rotCenter);
    camera.setPosition(camPos + 0.9 * (rotCenter - camPos));

    brayns.commitAndRender();
    BOOST_CHECK(compareTestImage("testdataallmini50advancedsimulation.png",
                                 brayns.getEngine().getFrameBuffer()));
}

BOOST_AUTO_TEST_CASE(render_sdf_circuit_and_compare)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    const char* argv[] = {app,
                          BBP_TEST_BLUECONFIG3,
                          "--disable-accumulation",
                          "--circuit-targets",
                          "Layer1",
                          "--morphology-dampen-branch-thickness-changerate",
                          "--morphology-use-sdf-geometries",
                          "--samples-per-pixel",
                          "16"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);

    const auto rotCenter = brayns.getEngine()
                               .getScene()
                               .getModel(0)
                               ->getTransformation()
                               .getRotationCenter();

    auto& camera = brayns.getEngine().getCamera();
    const auto camPos = camera.getPosition();

    camera.setTarget(rotCenter);
    camera.setPosition(camPos + 0.92 * (rotCenter - camPos));

    brayns.commitAndRender();
    BOOST_CHECK(compareTestImage("testSdfCircuit.png",
                                 brayns.getEngine().getFrameBuffer()));
}

BOOST_AUTO_TEST_CASE(render_circuit_with_basic_simulation_renderer)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const auto transfer_file = BRAYNS_TESTDATA_PATH "bbp.1dt";

    const char* app = testSuite.argv[0];
    const char* argv[] = {app,
                          "--circuit-config",
                          BBP_TEST_BLUECONFIG3,
                          "--circuit-targets",
                          "allmini50",
                          "--circuit-report",
                          "voltages",
                          "--renderer",
                          "basic_simulation",
                          "--samples-per-pixel",
                          "16",
                          "--color-map-file",
                          transfer_file,
                          "--color-map-range",
                          "-80",
                          "-10",
                          "--animation-frame",
                          "95",
                          "--synchronous-mode"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);

    const auto rotCenter = brayns.getEngine()
                               .getScene()
                               .getModel(0)
                               ->getTransformation()
                               .getRotationCenter();

    auto& camera = brayns.getEngine().getCamera();
    const auto camPos = camera.getPosition();

    camera.setTarget(rotCenter);
    camera.setPosition(camPos + 0.9 * (rotCenter - camPos));

    brayns.commitAndRender();
    BOOST_CHECK(compareTestImage("testdataallmini50basicsimulation.png",
                                 brayns.getEngine().getFrameBuffer()));
}
