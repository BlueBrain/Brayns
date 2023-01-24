/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <doctest/doctest.h>

#include <tests/helpers/BraynsTestUtils.h>
#include <tests/helpers/ImageValidator.h>

TEST_CASE("Perspective camera")
{
    auto args = "brayns";
    auto brayns = brayns::Brayns(1, &args);

    auto utils = BraynsTestUtils(brayns);
    utils.createDefaultScene();
    utils.adjustPerspectiveView();

    brayns.commitAndRender();

    CHECK(ImageValidator::validate(brayns.getEngine(), "test_perspective_camera.png"));

    auto &engine = brayns.getEngine();
    auto &camera = engine.getCamera();
    auto perspective = *camera.as<brayns::Perspective>();

    auto higherFovy = perspective;
    higherFovy.fovy *= 1.5f;
    camera.set(higherFovy);
    brayns.commitAndRender();
    CHECK(ImageValidator::validate(brayns.getEngine(), "test_perspective_camera_fovy.png"));
}

TEST_CASE("Orthographic camera")
{
    auto args = "brayns";
    auto brayns = brayns::Brayns(1, &args);

    auto utils = BraynsTestUtils(brayns);
    utils.createDefaultScene();
    utils.adjustOrthographicView();

    brayns.commitAndRender();

    CHECK(ImageValidator::validate(brayns.getEngine(), "test_orthographic_camera.png"));
}
