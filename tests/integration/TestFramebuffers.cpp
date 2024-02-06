/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <brayns/engine/framebuffer/types/ProgressiveFrameHandler.h>
#include <brayns/engine/framebuffer/types/StaticFrameHandler.h>
#include <brayns/engine/geometry/types/Sphere.h>
#include <brayns/engine/renderer/types/Interactive.h>

#include <tests/helpers/BraynsTestUtils.h>
#include <tests/helpers/ImageValidator.h>

TEST_CASE("Static framebuffer")
{
    auto utils = BraynsTestUtils();
    utils.createDefaultScene();
    utils.adjustPerspectiveView();
    utils.setRenderer(brayns::Interactive{3});

    auto &framebuffer = utils.getEngine().getFramebuffer();
    framebuffer.setFrameHandler(std::make_unique<brayns::StaticFrameHandler>());

    CHECK(ImageValidator::validate(utils.render(), "test_static_framebuffer.png"));
}

TEST_CASE("Progressive framebuffer")
{
    auto utils = BraynsTestUtils();
    utils.createDefaultScene();
    utils.adjustPerspectiveView();
    utils.setRenderer(brayns::Interactive{3});

    auto &framebuffer = utils.getEngine().getFramebuffer();
    framebuffer.setFrameHandler(std::make_unique<brayns::ProgressiveFrameHandler>());

    CHECK(ImageValidator::validate(utils.render(), "test_progressive_framebuffer_lowres.png"));
    CHECK(ImageValidator::validate(utils.render(), "test_progressive_framebuffer_highres.png"));

    utils.addGeometry(brayns::Sphere{brayns::Vector3f(0.f), 1.f});
    CHECK(ImageValidator::validate(utils.render(), "test_progressive_framebuffer_trigger_lowres_1.png"));

    utils.addGeometry(brayns::Sphere{brayns::Vector3f(0.f, 7.f, 0.f), 1.f});
    CHECK(ImageValidator::validate(utils.render(), "test_progressive_framebuffer_trigger_lowres_2.png"));

    CHECK(ImageValidator::validate(utils.render(), "test_progressive_framebuffer_trigger_highres.png"));
}
