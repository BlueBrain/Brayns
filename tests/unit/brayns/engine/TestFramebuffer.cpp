/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <brayns/engine/framebuffer/Framebuffer.h>
#include <brayns/engine/framebuffer/types/ProgressiveFrameHandler.h>
#include <brayns/engine/framebuffer/types/StaticFrameHandler.h>

#include <tests/unit/PlaceholderEngine.h>

TEST_CASE("Progressive frame handler")
{
    BRAYNS_TESTS_PLACEHOLDER_ENGINE

    SUBCASE("Constructor")
    {
        CHECK_NOTHROW(brayns::ProgressiveFrameHandler(4));
        CHECK_THROWS_AS(brayns::ProgressiveFrameHandler(0), std::invalid_argument);
    }
    SUBCASE("Commit")
    {
        auto handler = brayns::ProgressiveFrameHandler();
        CHECK(handler.commit());
        CHECK(!handler.commit());
    }
    SUBCASE("Frame size")
    {
        auto handler = brayns::ProgressiveFrameHandler();
        auto frameSize = brayns::Vector2ui(400, 400);
        handler.setFrameSize(frameSize);
        CHECK(handler.commit());

        handler.incrementAccumFrames();
        auto image = handler.getImage(brayns::FramebufferChannel::Color);
        CHECK(image.getWidth() == 100);
        CHECK(image.getHeight() == 100);

        handler.incrementAccumFrames();
        image = handler.getImage(brayns::FramebufferChannel::Color);
        CHECK(image.getWidth() == 400);
        CHECK(image.getHeight() == 400);
    }
    SUBCASE("Scale auto adjustment")
    {
        auto handler = brayns::ProgressiveFrameHandler();
        auto frameSize = brayns::Vector2ui(256, 120);
        handler.setFrameSize(frameSize);
        CHECK(handler.commit());

        handler.incrementAccumFrames();
        auto image = handler.getImage(brayns::FramebufferChannel::Color);
        CHECK(image.getWidth() == 256);
        CHECK(image.getHeight() == 120);

        handler.incrementAccumFrames();
        image = handler.getImage(brayns::FramebufferChannel::Color);
        CHECK(image.getWidth() == 256);
        CHECK(image.getHeight() == 120);
    }
    SUBCASE("Accumulation")
    {
        auto handler = brayns::ProgressiveFrameHandler();
        handler.commit();
        CHECK(handler.getAccumulationFrameCount() == 0);
        CHECK(!handler.hasNewAccumulationFrame());

        handler.incrementAccumFrames();
        handler.incrementAccumFrames();
        CHECK(handler.getAccumulationFrameCount() == 2);
        CHECK(handler.hasNewAccumulationFrame());

        handler.clear();
        CHECK(handler.getAccumulationFrameCount() == 0);
        CHECK(handler.hasNewAccumulationFrame());

        handler.resetNewAccumulationFrame();
        CHECK(handler.getAccumulationFrameCount() == 0);
        CHECK(!handler.hasNewAccumulationFrame());
    }
}

TEST_CASE("Static frame handler")
{
    BRAYNS_TESTS_PLACEHOLDER_ENGINE

    SUBCASE("Frame size")
    {
        auto handler = brayns::StaticFrameHandler();
        auto frameSize = brayns::Vector2ui(15, 15);
        CHECK_THROWS_AS(handler.setFrameSize(frameSize), std::invalid_argument);

        frameSize = brayns::Vector2ui(150, 0);
        CHECK_THROWS_AS(handler.setFrameSize(frameSize), std::invalid_argument);

        frameSize = brayns::Vector2ui(400, 400);
        CHECK_NOTHROW(handler.setFrameSize(frameSize));
        CHECK(handler.commit());

        auto image = handler.getImage(brayns::FramebufferChannel::Color);
        CHECK(image.getWidth() == 400);
        CHECK(image.getHeight() == 400);

        frameSize = brayns::Vector2ui(800, 100);
        handler.setFrameSize(frameSize);
        handler.commit();
        image = handler.getImage(brayns::FramebufferChannel::Color);
        CHECK(image.getWidth() == 800);
        CHECK(image.getHeight() == 100);
    }
    SUBCASE("Accumulation")
    {
        auto handler = brayns::StaticFrameHandler();
        handler.commit();
        CHECK(handler.getAccumulationFrameCount() == 0);
        CHECK(!handler.hasNewAccumulationFrame());

        handler.incrementAccumFrames();
        handler.incrementAccumFrames();
        CHECK(handler.getAccumulationFrameCount() == 2);
        CHECK(handler.hasNewAccumulationFrame());

        handler.clear();
        CHECK(handler.getAccumulationFrameCount() == 0);
        CHECK(handler.hasNewAccumulationFrame());

        handler.resetNewAccumulationFrame();
        CHECK(handler.getAccumulationFrameCount() == 0);
        CHECK(!handler.hasNewAccumulationFrame());
    }
}
