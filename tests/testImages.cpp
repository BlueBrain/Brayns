
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

#include <brayns/engine/Camera.h>
#include <brayns/engine/Engine.h>
#include <brayns/engine/FrameBuffer.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>

#define BOOST_TEST_MODULE testImages
#include <boost/test/unit_test.hpp>

#include "PDiffHelpers.h"

BOOST_AUTO_TEST_CASE(render_two_frames_and_compare_they_are_same)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();
    const char* app = testSuite.argv[0];
    const char* argv[] = {app, "--disable-accumulation", "demo"};
    const int argc = sizeof(argv) / sizeof(char*);
    brayns::Brayns brayns(argc, argv);

    brayns.commitAndRender();
    const auto oldImage =
        createPDiffRGBAImage(brayns.getEngine().getFrameBuffer());

    brayns.getEngine().getFrameBuffer().clear();
    brayns.commitAndRender();
    const auto newImage =
        createPDiffRGBAImage(brayns.getEngine().getFrameBuffer());

    BOOST_CHECK(pdiff::yee_compare(*oldImage, *newImage));
}

BOOST_AUTO_TEST_CASE(render_xyz_and_compare)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    const auto path = BRAYNS_TESTDATA_MODEL_MONKEY_PATH;
    const char* argv[] = {app, path, "--disable-accumulation"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);
    brayns.commitAndRender();
    BOOST_CHECK(compareTestImage("testdataMonkey.png",
                                 brayns.getEngine().getFrameBuffer()));

    auto model = brayns.getEngine().getScene().getModel(0);
    auto props = model->getProperties();
    props.updateProperty("radius", props.getProperty<double>("radius") / 2.);
    model->setProperties(props);

    brayns.commitAndRender();
    BOOST_CHECK(compareTestImage("testdataMonkey_smaller.png",
                                 brayns.getEngine().getFrameBuffer()));
}

BOOST_AUTO_TEST_CASE(render_demo_with_proximity_renderer)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    const char* argv[] = {
        app,   "--renderer", "proximity",     "--samples-per-pixel",
        "256", "demo",       "--window-size", "400",
        "300"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);

    const brayns::Vector3f rotCenter = {0.5f, 0.5f, 0.5f};

    auto& camera = brayns.getEngine().getCamera();
    const auto camPos = camera.getPosition();

    camera.setOrientation(brayns::Quaterniond(0.0, 0.0, 0.0, 1.0));
    camera.setPosition(camPos - (rotCenter - camPos));

    brayns.commitAndRender();
    pdiff::PerceptualDiffParameters params;
    params.luminance_only = true;
    BOOST_CHECK(compareTestImage("testdemoproximity.png",
                                 brayns.getEngine().getFrameBuffer(), params));
}

BOOST_AUTO_TEST_CASE(render_protein_and_compare)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    const char* argv[] = {app, BRAYNS_TESTDATA_MODEL_PDB_PATH,
                          "--disable-accumulation"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);
    brayns.commitAndRender();
    BOOST_CHECK(compareTestImage("testdataProtein.png",
                                 brayns.getEngine().getFrameBuffer()));
}

BOOST_AUTO_TEST_CASE(render_protein_in_stereo_and_compare)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    const char* argv[] = {app, BRAYNS_TESTDATA_MODEL_PDB_PATH,
                          "--disable-accumulation", "--stereo"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);
    brayns.commitAndRender();
    BOOST_CHECK(compareTestImage("testdataProtein_left_eye.png",
                                 *brayns.getEngine().getFrameBuffers()[0]));
    BOOST_CHECK(compareTestImage("testdataProtein_right_eye.png",
                                 *brayns.getEngine().getFrameBuffers()[1]));
}
