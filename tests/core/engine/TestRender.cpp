/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include <stdexcept>

#include <doctest.h>

#include <rkcommon/utility/SaveImage.h>

#include <brayns/core/engine/Device.h>

using namespace brayns;
using namespace brayns::experimental;

TEST_CASE("Render")
{
    auto error = std::string();

    auto level = LogLevel::Error;
    auto handler = [&](const auto &record) { error = record.message; };
    auto logger = Logger("Test", level, handler);

    auto device = createDevice(logger);

    auto width = 480;
    auto height = 360;

    auto toneMapper = device.create<ToneMapper>({});

    auto imageOperations = std::vector<ImageOperation>{toneMapper};

    auto framebuffer = device.create<Framebuffer>({
        .width = std::size_t(width),
        .height = std::size_t(height),
        .format = FramebufferFormat::Srgba8,
        .channels = {FramebufferChannel::Color},
        .operations = imageOperations,
    });

    auto material = device.create<ScivisMaterial>({});

    auto materials = std::vector<Material>{material};

    auto renderer = device.create<ScivisRenderer>({.base = {.materials = materials}});

    auto camera = device.create<PerspectiveCamera>({
        .fovy = 45.0F,
        .aspectRatio = float(width) / float(height),
    });

    auto points = std::vector<Vector4>{{0, 0, 3, 0.25F}, {1, 0, 3, 0.25F}, {1, 1, 3, 0.25F}};
    auto colors = std::vector<Color4>{{1, 0, 0, 1}, {0, 0, 1, 1}, {0, 1, 0, 1}};

    auto spheres = device.create<Spheres>({.points = points});

    auto materialIndices = std::vector<std::uint32_t>{0};

    auto model = device.create<GeometricModel>({
        .geometry = spheres,
        .materials = {.rendererIndices = materialIndices, .colors = colors},
        .id = 0,
    });

    auto light = device.create<AmbientLight>({});

    auto models = std::vector<GeometricModel>{model};
    auto lights = std::vector<Light>{light};

    auto group = device.create<Group>({.geometries = models, .lights = lights});

    auto instance = device.create<Instance>({.group = group, .transform = {}, .id = 0});

    auto instances = std::vector<Instance>{instance};

    auto world = device.create<World>({instances});

    CHECK_EQ(world.getBounds(), instance.getBounds());
    CHECK_EQ(world.getBounds(), group.getBounds());

    auto future = device.render({
        .framebuffer = framebuffer,
        .renderer = renderer,
        .camera = camera,
        .world = world,
    });
    auto duration = future.waitAndGetDuration();

    CHECK(duration > 0);
    CHECK(future.getProgress() == 1);

    auto data = framebuffer.map(FramebufferChannel::Color);

    /*rkcommon::utility::writePPM("test.ppm", width, height, static_cast<const std::uint32_t *>(data));*/

    framebuffer.unmap(data);

    CHECK(error.empty());
}
