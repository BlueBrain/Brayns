
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

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "tests/doctest.h"

TEST_CASE("simple_circuit")
{
    const char* argv[] = {"simple_circuit",
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
    CHECK(compareTestImage("testdataLayer1.png",
                           brayns.getEngine().getFrameBuffer()));
}

TEST_CASE("circuit_with_color_by_mtype")
{
    const char* argv[] = {
        "circuit_with_color_by_mtype",
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
    CHECK(compareTestImage("testdataMiniColumn0_mtypes.png",
                           brayns.getEngine().getFrameBuffer()));
}

void testSdfGeometries(bool dampened, const char* app)
{
    auto argv = std::vector<const char*>{app,
                                         BBP_TEST_BLUECONFIG3,
                                         "--disable-accumulation",
                                         "--samples-per-pixel",
                                         "16",
                                         "--plugin"};
    if (dampened)
        argv.push_back(
            "braynsCircuitViewer --targets Layer1 --use-sdf-geometries"
            "--disable-sdf-bezier-curves --dampen-branch-thickness-changerate");
    else
        argv.push_back(
            "braynsCircuitViewer --targets Layer1 --use-sdf-geometries"
            " --disable-sdf-bezier-curves");

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
    CHECK(compareTestImage(dampened ? "testDampenedSdfCircuit.png"
                                    : "testSdfCircuit.png",
                           brayns.getEngine().getFrameBuffer()));
}

TEST_CASE("circuit_with_sdf_geometries")
{
    testSdfGeometries(false, "circuit_with_sdf_geometries");
}

TEST_CASE("circuit_with_dampened_sdf_geometries")
{
    testSdfGeometries(true, "circuit_with_dampened_sdf_geometries");
}

TEST_CASE("circuit_with_sdf_bezier_curves")
{
    auto argv = std::vector<const char*>{"circuit_with_sdf_bezier_curves",
                                         BBP_TEST_BLUECONFIG3,
                                         "--disable-accumulation",
                                         "--samples-per-pixel",
                                         "16",
                                         "--plugin"};

    argv.push_back("braynsCircuitViewer --targets Layer1 --use-sdf-geometries");

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

    CHECK(compareTestImage("testSdfBezierCurvesCircuit.png",
                           brayns.getEngine().getFrameBuffer()));
}
