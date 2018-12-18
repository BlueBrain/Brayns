
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

#include "CircuitViewer/tests/paths.h"
#include "tests/PDiffHelpers.h"

#include <brayns/Brayns.h>

#include <brayns/common/simulation/AbstractSimulationHandler.h>
#include <brayns/engine/Camera.h>
#include <brayns/engine/Engine.h>
#include <brayns/engine/FrameBuffer.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>

#include <brayns/parameters/ParametersManager.h>

#include <BBP/TestDatasets.h>

#define BOOST_TEST_MODULE braynsTestData
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(simple_circuit)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    const char* argv[] = {app,
                          BBP_TEST_BLUECONFIG3,
                          "--disable-accumulation",
                          "--samples-per-pixel",
                          "16",
                          "--plugin",
                          "braynsCircuitViewer --targets Layer1"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);
    brayns.getParametersManager().getRenderingParameters().setCurrentRenderer(
        "basic");
    brayns.commitAndRender();
    BOOST_CHECK(compareTestImage("testdataLayer1.png",
                                 brayns.getEngine().getFrameBuffer()));
}

BOOST_AUTO_TEST_CASE(circuit_with_color_by_mtype)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    const char* argv[] = {
        app,
        BBP_TEST_BLUECONFIG3,
        "--disable-accumulation",
        "--samples-per-pixel",
        "16",
        "--plugin",
        "braynsCircuitViewer --targets MiniColumn_0 --color-scheme by-mtype"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);
    brayns.getParametersManager().getRenderingParameters().setCurrentRenderer(
        "basic");
    brayns.getEngine().getScene().setMaterialsColorMap(
        brayns::MaterialsColorMap::gradient);
    brayns.commitAndRender();
    BOOST_CHECK(compareTestImage("testdataMiniColumn0_mtypes.png",
                                 brayns.getEngine().getFrameBuffer()));
}

BOOST_AUTO_TEST_CASE(circuit_with_simulation_mapping)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    const char* argv[] = {app, BBP_TEST_BLUECONFIG3, "--samples-per-pixel",
                          "16", "--animation-frame", "50", "--plugin",
                          "braynsCircuitViewer --targets allmini50 --report "
                          "voltages --synchronous-mode"};

    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);

    auto modelDesc = brayns.getEngine().getScene().getModel(0);
    const auto rotCenter = modelDesc->getTransformation().getRotationCenter();

    auto& camera = brayns.getEngine().getCamera();
    const auto camPos = camera.getPosition();

    std::cout << "Rot Center: " << rotCenter << std::endl;
    std::cout << "Cam Pos: " << camPos << std::endl;

    camera.setOrientation(brayns::Quaterniond(0.0, 0.0, 0.0, 1.0));
    camera.setPosition(camPos + 0.9 * (rotCenter - camPos));

    modelDesc->getModel().getTransferFunction().setValuesRange({-66, -62});

    brayns.commit();
    modelDesc->getModel().getSimulationHandler()->waitReady();
    brayns.commitAndRender();
    BOOST_CHECK(compareTestImage("testdataallmini50basicsimulation.png",
                                 brayns.getEngine().getFrameBuffer()));
}

void testSdfGeometries(bool dampened)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    auto argv = std::vector<const char*>{app,
                                         BBP_TEST_BLUECONFIG3,
                                         "--disable-accumulation",
                                         "--samples-per-pixel",
                                         "16",
                                         "--plugin"};
    if (dampened)
        argv.push_back(
            "braynsCircuitViewer --targets Layer1 --use-sdf-geometries "
            "--dampen-branch-thickness-changerate");
    else
        argv.push_back(
            "braynsCircuitViewer --targets Layer1 --use-sdf-geometries");

    const int argc = argv.size();

    brayns::Brayns brayns(argc, argv.data());

    brayns.getParametersManager().getRenderingParameters().setCurrentRenderer(
        "basic");
    const auto rotCenter = brayns.getEngine()
                               .getScene()
                               .getModel(0)
                               ->getTransformation()
                               .getRotationCenter();

    auto& camera = brayns.getEngine().getCamera();
    const auto camPos = camera.getPosition();

    camera.setOrientation(brayns::Quaterniond(0.0, 0.0, 0.0, 1.0));
    camera.setPosition(camPos + 0.92 * (rotCenter - camPos));

    brayns.commitAndRender();
    BOOST_CHECK(compareTestImage(dampened ? "testDampenedSdfCircuit.png"
                                          : "testSdfCircuit.png",
                                 brayns.getEngine().getFrameBuffer()));
}

BOOST_AUTO_TEST_CASE(circuit_with_sdf_geometries)
{
    testSdfGeometries(false);
}

BOOST_AUTO_TEST_CASE(circuit_with_dampened_sdf_geometries)
{
    testSdfGeometries(true);
}
