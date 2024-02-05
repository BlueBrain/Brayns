/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include "helpers/ImageValidator.h"

TEST_CASE("render_two_frames_and_compare_they_are_same")
{
    const char* argv[] = {"testImages", "--disable-accumulation", "demo"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);
    auto& engine = brayns.getEngine();
    auto& framebuffer = engine.getFrameBuffer();

    brayns.commitAndRender();
    auto oldImage = framebuffer.getImage();

    framebuffer.clear();

    brayns.commitAndRender();
    auto newImage = framebuffer.getImage();

    CHECK(ImageValidator::validate(oldImage, newImage));
}

TEST_CASE("render_xyz_and_compare")
{
    const auto path = BRAYNS_TESTDATA_MODEL_MONKEY_PATH;
    const char* argv[] = {"testImages", path, "--disable-accumulation"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);
    auto& engine = brayns.getEngine();

    brayns.commitAndRender();
    CHECK(ImageValidator::validate(engine, "testdataMonkey.png"));

    auto model = engine.getScene().getModel(0);
    auto properties = model->getProperties();
    properties.update("radius", properties["radius"].as<double>() / 2.);
    model->setProperties(properties);

    engine.getScene().markModified();

    brayns.commitAndRender();
    CHECK(ImageValidator::validate(engine, "testdataMonkey_smaller.png"));
}

TEST_CASE("render_protein_and_compare")
{
    const char* argv[] = {"testImages", BRAYNS_TESTDATA_MODEL_PDB_PATH,
                          "--disable-accumulation"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);
    auto& engine = brayns.getEngine();

    brayns.commitAndRender();
    CHECK(ImageValidator::validate(engine, "testdataProtein.png"));
}

TEST_CASE("render_ply_and_compare")
{
    const auto path = BRAYNS_TESTDATA_MODEL_LUCY_PATH;
    const char* argv[] = {"testImages", path, "--disable-accumulation"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);
    auto& engine = brayns.getEngine();

    brayns.commitAndRender();
    CHECK(ImageValidator::validate(engine, "testdataLucy.png"));
}
