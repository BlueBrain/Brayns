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

#include <tests/helpers/BraynsTestUtils.h>
#include <tests/helpers/ImageValidator.h>

namespace
{
class FramebufferChannelTester
{
public:
    static void run(brayns::FramebufferChannel channel, const std::string &referenceImage)
    {
        auto utils = BraynsTestUtils();
        utils.createDefaultScene();
        utils.adjustPerspectiveView();

        auto &engine = utils.getEngine();
        auto &frameBuffer = engine.getFramebuffer();
        frameBuffer.setChannels({channel});
        frameBuffer.setFormat(brayns::PixelFormat::RgbaF32);
        frameBuffer.setToneMappingEnabled(false);

        engine.commitAndRender();
        auto image = frameBuffer.getImage(channel);
        CHECK(ImageValidator::validate(image, referenceImage));
    }
};
}

TEST_CASE("Framebuffer channels")
{
    SUBCASE("Depth")
    {
        FramebufferChannelTester::run({brayns::FramebufferChannel::Depth}, "test_framebuffer_channel_depth.exr");
    }
    SUBCASE("Albedo")
    {
        FramebufferChannelTester::run({brayns::FramebufferChannel::Albedo}, "test_framebuffer_channel_albedo.exr");
    }
    SUBCASE("Normal")
    {
        FramebufferChannelTester::run({brayns::FramebufferChannel::Normal}, "test_framebuffer_channel_normal.exr");
    }
}
