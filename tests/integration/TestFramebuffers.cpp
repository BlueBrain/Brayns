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

#include <brayns/engine/framebuffer/types/ProgressiveFrameHandler.h>
#include <brayns/engine/framebuffer/types/StaticFrameHandler.h>
#include <brayns/engine/renderer/types/Interactive.h>

#include <tests/helpers/BraynsTestUtils.h>
#include <tests/helpers/ImageValidator.h>

TEST_CASE("Static framebuffer")
{
    auto params = std::array<const char *, 1>{"brayns"};
    auto brayns = brayns::Brayns(1, params.data());

    auto utils = BraynsTestUtils(brayns);
    utils.createDefaultScene();
    utils.adjustPerspectiveView();
    utils.setRenderer(brayns::Interactive{3});

    auto &framebuffer = brayns.getEngine().getFramebuffer();
    framebuffer.setFrameHandler(std::make_unique<brayns::StaticFrameHandler>());

    CHECK(ImageValidator::validate(utils.render(), "test_static_framebuffer.png"));
}

TEST_CASE("Progressive framebuffer")
{
    auto params = std::array<const char *, 1>{"brayns"};
    auto brayns = brayns::Brayns(1, params.data());

    auto utils = BraynsTestUtils(brayns);
    utils.createDefaultScene();
    utils.adjustPerspectiveView();
    utils.setRenderer(brayns::Interactive{3});

    auto &framebuffer = brayns.getEngine().getFramebuffer();
    framebuffer.setFrameHandler(std::make_unique<brayns::ProgressiveFrameHandler>());

    CHECK(ImageValidator::validate(utils.render(), "test_progressive_framebuffer_lowres.png"));
    CHECK(ImageValidator::validate(utils.render(), "test_progressive_framebuffer_highres.png"));
}
