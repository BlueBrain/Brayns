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

TEST_CASE("Render")
{
    auto level = LogLevel::Error;
    level = LogLevel::Debug;
    // auto handler = [&](const auto &record) { throw std::runtime_error(std::string(record.message)); };
    auto handler = [&](const auto &record) { std::cout << record.message << '\n'; };
    auto logger = Logger("Test", level, handler);

    auto api = loadGraphicsApi();

    auto device = api.createDevice(logger);

    auto width = 480;
    auto height = 360;

    auto toneMapper = device.create<ToneMapper>();
    toneMapper.commit();

    auto imageOperations = std::vector<ImageOperation>{toneMapper};

    auto framebuffer = device.createFramebuffer({
        .width = std::size_t(width),
        .height = std::size_t(height),
    });
    framebuffer.setImageOperations(imageOperations);
    framebuffer.commit();

    auto material = device.create<ObjMaterial>();
    material.commit();

    auto materials = std::vector<BaseMaterial>{material};

    auto renderer = device.create<ScivisRenderer>();
    renderer.enableShadows(true);
    renderer.setPixelSamples(1);
    renderer.setAoSamples(0);
    renderer.setMaterials(materials);
    renderer.commit();

    auto camera = device.create<PerspectiveCamera>();
    camera.setAspectRatio(float(width) / float(height));
    camera.setFovy(45);
    camera.setTransform(toAffine({.translation = {0, 0, -1}}));
    camera.commit();

    auto positions = std::vector<Vector3>{{0, 0, 3}, {1, 0, 3}, {1, 1, 3}};
    auto radii = std::vector<float>{0.25F, 0.25F, 0.25F};
    auto colors = std::vector<Color4>{{1, 0, 0, 1}, {0, 0, 1, 1}, {0, 1, 0, 1}};

    auto spheres = device.create<SphereGeometry>();
    spheres.setPositions(positions);
    spheres.setRadii(radii);
    spheres.commit();

    auto model = device.createGeometryModel();
    model.setGeometry(spheres);
    model.setPrimitiveColors(colors);
    model.setId(0);
    model.setMaterial(0);
    model.commit();

    auto models = std::vector<GeometricModel>{model};

    auto light = device.create<AmbientLight>();
    light.setIntensity(1);
    light.commit();

    auto lights = std::vector<BaseLight>{light};

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

    auto task = device.render({
        .framebuffer = framebuffer,
        .renderer = renderer,
        .camera = camera,
        .world = world,
    });
    task.wait();

    auto data = framebuffer.map(Channel::Color);
    rkcommon::utility::writePPM(
        "/home/acfleury/source/repos/Brayns/test.ppm",
        width,
        height,
        static_cast<const std::uint32_t *>(data));
    framebuffer.unmap(data);
}
