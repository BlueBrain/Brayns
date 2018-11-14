
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

#include <brayns/common/camera/Camera.h>
#include <brayns/common/engine/Engine.h>
#include <brayns/common/renderer/FrameBuffer.h>
#include <brayns/common/scene/Model.h>
#include <brayns/common/scene/Scene.h>

#include <BBP/TestDatasets.h>

#define BOOST_TEST_MODULE braynsTestData
#include <boost/test/unit_test.hpp>

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

    const char* app = testSuite.argv[0];
    const char* argv[] = {app,
                          BBP_TEST_BLUECONFIG3,
                          "--circuit-targets",
                          "allmini50",
                          "--circuit-report",
                          "voltages",
                          "--renderer",
                          "advanced_simulation",
                          "--samples-per-pixel",
                          "16",
                          "--animation-frame",
                          "50",
                          "--synchronous-mode"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);

    auto model = brayns.getEngine().getScene().getModel(0);
    const auto rotCenter = model->getTransformation().getRotationCenter();

    auto& camera = brayns.getEngine().getCamera();
    const auto camPos = camera.getPosition();

    std::cout << "Rot Center: " << rotCenter << std::endl;
    std::cout << "Cam Pos: " << camPos << std::endl;

    camera.setOrientation(brayns::Quaterniond(0.0, 0.0, 0.0, 1.0));
    camera.setPosition(camPos + 0.9 * (rotCenter - camPos));

    auto& tf = model->getModel().getTransferFunction();
    tf.setValuesRange({-66, -62});
    tf.setColorMap(
        {"test_f",
         {{0.996078431372549, 0.9294117647058824, 0.8666666666666667},
          {0.996078431372549, 0.9019607843137255, 0.8117647058823529},
          {0.9921568627450981, 0.8627450980392157, 0.7333333333333333},
          {0.9921568627450981, 0.8235294117647058, 0.6549019607843137},
          {0.9921568627450981, 0.7686274509803922, 0.5568627450980392},
          {0.9921568627450981, 0.7058823529411765, 0.4549019607843137},
          {0.9921568627450981, 0.6431372549019608, 0.36470588235294116},
          {0.9921568627450981, 0.5803921568627451, 0.2784313725490196},
          {0.9803921568627451, 0.5176470588235295, 0.19607843137254902},
          {0.9568627450980393, 0.45098039215686275, 0.12156862745098039},
          {0.9254901960784314, 0.38823529411764707, 0.058823529411764705},
          {0.8823529411764706, 0.3254901960784314, 0.027450980392156862},
          {0.8274509803921568, 0.27058823529411763, 0.00392156862745098},
          {0.7333333333333333, 0.23921568627450981, 0.00784313725490196},
          {0.6392156862745098, 0.20784313725490197, 0.011764705882352941},
          {0.5686274509803921, 0.1803921568627451, 0.011764705882352941}}});
    brayns::Vector2ds controlPoints;
    controlPoints.reserve(16);
    controlPoints.push_back({0., 1.});
    for (size_t i = 0; i <= 15; ++i)
        controlPoints.push_back({i / 15., 1.});
    tf.setControlPoints(controlPoints);

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

    camera.setOrientation(brayns::Quaterniond(0.0, 0.0, 0.0, 1.0));
    camera.setPosition(camPos + 0.92 * (rotCenter - camPos));

    brayns.commitAndRender();
    BOOST_CHECK(compareTestImage("testSdfCircuit.png",
                                 brayns.getEngine().getFrameBuffer()));
}
