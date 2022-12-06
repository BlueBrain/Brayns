/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/engine/core/Engine.h>
#include <brayns/engine/framebuffer/Framebuffer.h>
#include <brayns/engine/framebuffer/types/ProgressiveFrameHandler.h>
#include <brayns/engine/framebuffer/types/StaticFrameHandler.h>

TEST_CASE("Progressive framebuffer")
{
    auto params = brayns::ParametersManager(0, nullptr);
    auto engine = brayns::Engine(params);
    (void)engine;

    SUBCASE("Constructor")
    {
        auto goodHandler = std::make_unique<brayns::ProgressiveFrameHandler>(10);
        CHECK_NOTHROW(brayns::Framebuffer(std::move(goodHandler)));

        auto badHandler = std::make_unique<brayns::ProgressiveFrameHandler>(0);
        CHECK_THROWS_AS(brayns::Framebuffer(std::move(badHandler)), std::invalid_argument);
    }
}
