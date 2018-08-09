
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

#define BOOST_TEST_MODULE braynsTestData
#include <boost/test/unit_test.hpp>

#include "PDiffHelpers.h"

constexpr auto PDB_FILE = BRAYNS_TESTDATA_PATH "1bna.pdb";

BOOST_AUTO_TEST_CASE(render_two_frames_and_compare_they_are_same)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();
    const char* app = testSuite.argv[0];
    const char* argv[] = {app, "--accumulation", "off", "demo"};
    const int argc = sizeof(argv) / sizeof(char*);
    brayns::Brayns brayns(argc, argv);

    brayns.render();
    const auto oldImage =
        createPDiffRGBAImage(brayns.getEngine().getFrameBuffer());

    brayns.getEngine().getFrameBuffer().clear();
    brayns.render();
    const auto newImage =
        createPDiffRGBAImage(brayns.getEngine().getFrameBuffer());

    BOOST_CHECK(pdiff::yee_compare(*oldImage, *newImage));
}

BOOST_AUTO_TEST_CASE(render_xyz_and_compare)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    const auto path = BRAYNS_TESTDATA_MODEL_MONKEY_PATH;
    const char* argv[] = {app, path, "--accumulation", "off"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);
    brayns.render();
    BOOST_CHECK(compareTestImage("testdataMonkey.png",
                                 brayns.getEngine().getFrameBuffer()));
}

#ifdef BRAYNS_USE_BBPTESTDATA
BOOST_AUTO_TEST_CASE(render_circuit_and_compare)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    const char* argv[] = {app,
                          BBP_TEST_BLUECONFIG3,
                          "--accumulation",
                          "off",
                          "--circuit-targets",
                          "Layer1",
                          "--samples-per-pixel",
                          "16"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);
    brayns.render();
    BOOST_CHECK(compareTestImage("testdataLayer1.png",
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
                          "simulation",
                          "--samples-per-pixel",
                          "16",
                          "--color-map-file",
                          transfer_file,
                          "--color-map-range",
                          "-66",
                          "-62",
                          "--animation-frame",
                          "50"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);

    brayns.render();
    BOOST_CHECK(compareTestImage("testdataallmini50color.png",
                                 brayns.getEngine().getFrameBuffer()));
}

BOOST_AUTO_TEST_CASE(render_sdf_circuit_and_compare)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    const char* argv[] = {app,
                          BBP_TEST_BLUECONFIG3,
                          "--accumulation",
                          "off",
                          "--circuit-targets",
                          "Layer1",
                          "--morphology-dampen-branch-thickness-changerate",
                          "true",
                          "--morphology-use-sdf-geometries",
                          "true",
                          "--samples-per-pixel",
                          "16"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);
    brayns.render();
    BOOST_CHECK(compareTestImage("testSdfCircuit.png",
                                 brayns.getEngine().getFrameBuffer()));
}
#endif

BOOST_AUTO_TEST_CASE(render_protein_and_compare)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    const char* argv[] = {app, PDB_FILE, "--accumulation", "off"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);
    brayns.render();
    BOOST_CHECK(compareTestImage("testdataProtein.png",
                                 brayns.getEngine().getFrameBuffer()));
}

BOOST_AUTO_TEST_CASE(render_protein_in_stereo_and_compare)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    const char* argv[] = {app, PDB_FILE, "--accumulation", "off"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);
    brayns.getEngine().getCamera().updateProperty("stereoMode", 3);
    brayns.render();
    BOOST_CHECK(compareTestImage("testdataProteinStereo.png",
                                 brayns.getEngine().getFrameBuffer()));
}
