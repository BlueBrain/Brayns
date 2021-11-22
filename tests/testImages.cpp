/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
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

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "PDiffHelpers.h"

TEST_CASE("render_two_frames_and_compare_they_are_same")
{
    const char* argv[] = {"testImages", "--disable-accumulation", "demo"};
    const int argc = sizeof(argv) / sizeof(char*);
    brayns::Brayns brayns(argc, argv);

    brayns.commitAndRender();
    const auto oldImage =
        createPDiffRGBAImage(brayns.getEngine().getFrameBuffer());

    brayns.getEngine().getFrameBuffer().clear();
    brayns.commitAndRender();
    const auto newImage =
        createPDiffRGBAImage(brayns.getEngine().getFrameBuffer());

    CHECK(pdiff::yee_compare(*oldImage, *newImage));
}

TEST_CASE("render_xyz_and_compare")
{
    const auto path = BRAYNS_TESTDATA_MODEL_MONKEY_PATH;
    const char* argv[] = {"testImages", path, "--disable-accumulation"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);
    brayns.commitAndRender();
    CHECK(compareTestImage("testdataMonkey.png",
                           brayns.getEngine().getFrameBuffer()));

    auto model = brayns.getEngine().getScene().getModel(0);
    auto props = model->getProperties();
    props.update("radius", props["radius"].as<double>() / 2.);
    model->setProperties(props);

    brayns.getEngine().getScene().markModified();

    brayns.commitAndRender();
    CHECK(compareTestImage("testdataMonkey_smaller.png",
                           brayns.getEngine().getFrameBuffer()));
}

TEST_CASE("render_protein_and_compare")
{
    const char* argv[] = {"testImages", BRAYNS_TESTDATA_MODEL_PDB_PATH,
                          "--disable-accumulation"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);
    brayns.commitAndRender();
    CHECK(compareTestImage("testdataProtein.png",
                           brayns.getEngine().getFrameBuffer()));
}

TEST_CASE("render_protein_in_stereo_and_compare")
{
    const char* argv[] = {"testImages", BRAYNS_TESTDATA_MODEL_PDB_PATH,
                          "--disable-accumulation", "--stereo"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);
    brayns.commitAndRender();
    CHECK(compareTestImage("testdataProtein_left_eye.png",
                           *brayns.getEngine().getFrameBuffers()[0]));
    CHECK(compareTestImage("testdataProtein_right_eye.png",
                           *brayns.getEngine().getFrameBuffers()[1]));
}

TEST_CASE("render_ply_and_compare")
{
    const auto path = BRAYNS_TESTDATA_MODEL_LUCY_PATH;
    const char* argv[] = {"testImages", path, "--disable-accumulation"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);
    brayns.commitAndRender();
    CHECK(compareTestImage("testdataLucy.png",
                           brayns.getEngine().getFrameBuffer()));
}
