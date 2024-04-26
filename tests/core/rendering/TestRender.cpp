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

#include <brayns/core/rendering/GraphicsApi.h>

using namespace brayns;
using namespace brayns::experimental;

TEST_CASE("Render")
{
    auto error = std::string();

    auto level = LogLevel::Error;
    auto handler = [&](const auto &record) { error = record.message; };
    auto logger = Logger("Test", level, handler);

    auto api = loadGraphicsApi();

    auto device = api.createDevice(logger);

    auto width = 480;
    auto height = 360;

    auto toneMapper = device.createImageOperation<ToneMapper>();
    toneMapper.commit();

    auto imageOperations = std::vector<ImageOperation>{toneMapper};

    auto framebuffer = device.createFramebuffer({
        .width = std::size_t(width),
        .height = std::size_t(height),
        .format = FramebufferFormat::Srgba8,
        .channels = {FramebufferChannel::Color},
    });
    framebuffer.setImageOperations(imageOperations);
    framebuffer.commit();

    auto material = device.createMaterial<ObjMaterial>();
    material.setDiffuseColor({1, 1, 1});
    material.setSpecularColor({0, 0, 0});
    material.setTransparencyFilter({0, 0, 0});
    material.setShininess(10);
    material.commit();

    auto materials = std::vector<Material>{material};

    auto renderer = device.createRenderer<ScivisRenderer>();
    renderer.setBackgroundColor({1, 1, 1, 1});
    renderer.enableShadows(true);
    renderer.setPixelSamples(1);
    renderer.setAoSamples(0);
    renderer.setMaterials(materials);
    renderer.commit();

    auto camera = device.createCamera<PerspectiveCamera>();
    camera.setAspectRatio(float(width) / float(height));
    camera.setFovy(45);
    camera.setTransform(toAffine({.translation = {0, 0, -1}}));
    camera.setNearClip(0);
    camera.commit();

    auto positions = std::vector<Vector3>{{0, 0, 3}, {1, 0, 3}, {1, 1, 3}};
    auto radii = std::vector<float>{0.25F, 0.25F, 0.25F};
    auto colors = std::vector<Color4>{{1, 0, 0, 1}, {0, 0, 1, 1}, {0, 1, 0, 1}};

    auto spheres = device.createGeometry<Spheres>();
    spheres.setPositions(positions);
    spheres.setRadii(radii);
    spheres.commit();

    auto model = device.createGeometricModel();
    model.setGeometry(spheres);
    model.setPrimitiveColors(colors);
    model.setMaterial(0);
    model.setId(0);
    model.commit();

    auto models = std::vector<GeometricModel>{model};

    auto light = device.createLight<AmbientLight>();
    light.setIntensity(1);
    light.setColor({1, 1, 1});
    light.setVisible(true);
    light.commit();

    auto lights = std::vector<Light>{light};

    auto group = device.createGroup();
    group.setGeometries(models);
    group.setLights(lights);
    group.commit();

    auto instance = device.createInstance();
    instance.setGroup(group);
    instance.setTransform(toAffine({}));
    instance.setId(0);
    instance.commit();

    auto instances = std::vector<Instance>{instance};

    auto world = device.createWorld();
    world.setInstances(instances);
    world.commit();

    CHECK_EQ(world.getBounds(), instance.getBounds());
    CHECK_EQ(world.getBounds(), group.getBounds());

    auto task = device.render({
        .framebuffer = framebuffer,
        .renderer = renderer,
        .camera = camera,
        .world = world,
    });
    auto duration = task.waitAndGetDuration();

    CHECK(duration > 0);
    CHECK(task.getProgress() == 1);

    auto data = framebuffer.map(FramebufferChannel::Color);

    /*rkcommon::utility::writePPM(
        "test.ppm",
        width,
        height,
        static_cast<const std::uint32_t *>(data));*/

    auto pixels = static_cast<const std::uint32_t *>(data);

    auto sum = std::uint32_t(0);
    for (auto i = 0; i < width * height; ++i)
    {
        sum += pixels[i];
    }

    CHECK(sum > 0);

    framebuffer.unmap(data);

    CHECK(error.empty());
}
