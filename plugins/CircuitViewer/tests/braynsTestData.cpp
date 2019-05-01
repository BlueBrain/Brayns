
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
#ifdef BRAYNS_USE_NETWORKING
#include <jsonSerialization.h>
#endif

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

#ifdef BRAYNS_USE_NETWORKING
#include "tests/ClientServer.h"
#else
#endif

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#include <fstream>

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

#ifdef BRAYNS_USE_NETWORKING
BOOST_AUTO_TEST_CASE(circuit_with_simulation_mapping)
{
    const std::vector<const char*> argv = {
        BBP_TEST_BLUECONFIG3, "--samples-per-pixel", "16", "--animation-frame",
        "50", "--plugin",
        "braynsCircuitViewer --targets allmini50 --report "
        "voltages --synchronous-mode"};

    ClientServer clientServer(argv);

    auto modelDesc =
        clientServer.getBrayns().getEngine().getScene().getModel(0);
    const auto rotCenter = modelDesc->getTransformation().getRotationCenter();

    auto& camera = clientServer.getBrayns().getEngine().getCamera();
    const auto camPos = camera.getPosition();

    std::cout << "Rot Center: " << rotCenter << std::endl;
    std::cout << "Cam Pos: " << camPos << std::endl;

    camera.setOrientation(brayns::Quaterniond(1, 0, 0, 0));
    camera.setPosition(camPos + 0.9 * (rotCenter - camPos));

    modelDesc->getModel().getTransferFunction().setValuesRange({-66, -62});

    clientServer.getBrayns().commit();
    modelDesc->getModel().getSimulationHandler()->waitReady();
    clientServer.getBrayns().commitAndRender();
    BOOST_CHECK(compareTestImage(
        "testdataallmini50basicsimulation.png",
        clientServer.getBrayns().getEngine().getFrameBuffer()));

    brayns::SnapshotParams params;
    params.format = "png";
    params.samplesPerPixel = 16;
    params.size = clientServer.getBrayns()
                      .getParametersManager()
                      .getApplicationParameters()
                      .getWindowSize();
    params.animParams = std::make_unique<brayns::AnimationParameters>(
        clientServer.getBrayns()
            .getParametersManager()
            .getAnimationParameters());
    params.animParams->setFrame(42);

    auto image = clientServer.makeRequest<
        brayns::SnapshotParams, brayns::ImageGenerator::ImageBase64>("snapshot",
                                                                     params);

    BOOST_CHECK(compareBase64TestImage(
        image, "testdataallmini50basicsimulation_snapshot.png"));
    BOOST_CHECK(
        !compareBase64TestImage(image, "testdataallmini50basicsimulation.png"));
}
#endif

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

    camera.setOrientation(brayns::Quaterniond(1, 0, 0, 0));
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
