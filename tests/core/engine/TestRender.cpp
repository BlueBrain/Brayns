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

#include <brayns/core/engine/Camera.h>
#include <brayns/core/engine/Device.h>
#include <brayns/core/engine/Framebuffer.h>
#include <brayns/core/engine/GeometricModel.h>
#include <brayns/core/engine/Geometry.h>
#include <brayns/core/engine/ImageOperation.h>
#include <brayns/core/engine/Light.h>
#include <brayns/core/engine/Material.h>
#include <brayns/core/engine/Render.h>
#include <brayns/core/engine/Renderer.h>
#include <brayns/core/engine/Texture.h>
#include <brayns/core/engine/TransferFunction.h>
#include <brayns/core/engine/Volume.h>
#include <brayns/core/engine/VolumetricModel.h>
#include <brayns/core/engine/World.h>

using namespace brayns;

TEST_CASE("Object creation")
{
    auto error = std::string();

    auto handler = [&](const auto &record) { error = record.message; };
    auto logger = Logger("Test", LogLevel::Error, handler);

    auto device = createDevice(logger);

    createPerspectiveCamera(device, {});
    createOrthographicCamera(device, {});
    createPanoramicCamera(device, {});

    createData<int>(device, {1, 2, 3});
    allocateData2D<int>(device, {10, 10});
    allocateData3D<int>(device, {10, 10, 10});

    createToneMapper(device, {});
    createFramebuffer(device, {{100, 100}});

    auto volumeData = allocateData3D<float>(device, {10, 10, 10});
    std::ranges::fill(volumeData.getItems(), 1.0F);
    auto volume = createRegularVolume(device, volumeData, {});

    auto colors = createData<Color4>(device, {Color4{1, 0, 0, 1}, Color4{0, 0, 1, 1}});
    auto transferFunction = createLinearTransferFunction(device, {{0, 1}, colors});

    createVolumetricModel(device, {volume, transferFunction});

    auto triangles = std::vector<Vector3>{{0, 0, 0}, {1, 1, 1}, {2, 2, 2}};
    createTriangleMesh(device, {createData<Vector3>(device, triangles)});

    auto quads = std::vector<Vector3>{{0, 0, 0}, {1, 1, 1}, {2, 2, 2}, {3, 3, 3}};
    createQuadMesh(device, {createData<Vector3>(device, quads)});

    auto spheres = std::vector<Vector4>{{0, 0, 0, 1}, {1, 1, 1, 1}};
    auto geometry = createSpheres(device, {createData<Vector4>(device, spheres)});
    createDiscs(device, {createData<Vector4>(device, spheres)});

    createCylinders(device, {createData<Vector4>(device, spheres), createData<std::uint32_t>(device, {0})});
    createCurve(device, {createData<Vector4>(device, spheres), createData<std::uint32_t>(device, {0})});

    auto boxes = std::vector<Box3>{{{0, 0, 0}, {1, 1, 1}}};
    createBoxes(device, {createData<Box3>(device, boxes)});

    auto planes = std::vector<Vector4>{{1, 2, 3, 4}};
    createPlanes(device, {createData<Vector4>(device, planes)});

    createIsosurfaces(device, {volume, createData<float>(device, {1})});

    auto textureData2D = allocateData2D<Color4>(device, {10, 10});
    auto texture2D = createTexture2D(device, {textureData2D, TextureFormat::Rgba32F});
    createVolumeTexture(device, {volume, transferFunction});

    createDistantLight(device, {});
    createSphereLight(device, {});
    createSpotLight(device, {});
    createQuadLight(device, {});
    createCylinderLight(device, {});
    createHdriLight(device, {texture2D});
    createAmbientLight(device, {});
    createSunSkyLight(device, {});

    auto aoMaterial = createAoMaterial(device, {});
    auto scivisMaterial = createScivisMaterial(device, {});
    auto principled = createPrincipledMaterial(device, {});

    createAoRenderer(device, {{createData<Material>(device, {aoMaterial})}});
    createScivisRenderer(device, {{createData<Material>(device, {scivisMaterial})}});
    createPathTracer(device, {{createData<Material>(device, {principled})}});

    auto model = createGeometricModel(device, {geometry, {IndexInRenderer(0)}});
    auto group = createGroup(device, {createData<GeometricModel>(device, {model})});
    auto instance = createInstance(device, {group});
    createWorld(device, {createData<Instance>(device, {instance})});

    CHECK(error.empty());
}

TEST_CASE("Render")
{
    auto error = std::string();

    auto level = LogLevel::Error;
    auto handler = [&](const auto &record) { error = record.message; };
    auto logger = Logger("Test", level, handler);

    auto device = createDevice(logger);

    auto width = 480;
    auto height = 360;

    auto toneMapper = createToneMapper(device, {});

    auto framebuffer = createFramebuffer(
        device,
        {
            .resolution = {std::size_t(width), std::size_t(height)},
            .format = FramebufferFormat::Srgba8,
            .channels = {FramebufferChannel::Color},
            .operations = createData<ImageOperation>(device, {toneMapper}),
        });

    auto material = createScivisMaterial(device, {});

    auto renderer = createScivisRenderer(device, {{.materials = createData<Material>(device, {material})}});

    auto camera = createPerspectiveCamera(device, {}, {.fovy = 45.0F, .aspect = float(width) / float(height)});
    // auto camera = createOrthographicCamera(device, {}, {.height = 3, .aspect = float(width) / float(height)});

    auto sphereData = std::vector<Vector4>{{0, 0, 3, 0.25F}, {1, 0, 3, 0.25F}, {1, 1, 3, 0.25F}};
    auto colors = std::vector<Color4>{{1, 0, 0, 1}, {0, 0, 1, 1}, {0, 1, 0, 1}};

    auto spheres = createSpheres(device, {createData<Vector4>(device, sphereData)});

    auto model = createGeometricModel(
        device,
        {
            .geometry = spheres,
            .materials = {IndexInRenderer(0), createData<Color4>(device, colors)},
            .id = 0,
        });

    auto light = createAmbientLight(device, {});

    auto models = std::vector<GeometricModel>{model};
    auto lights = std::vector<Light>{light};

    auto group = createGroup(
        device,
        {
            .geometries = createData<GeometricModel>(device, {model}),
            .lights = createData<Light>(device, {light}),
        });

    auto instance = createInstance(device, {.group = group, .transform = {}, .id = 0});

    auto world = createWorld(device, {createData<Instance>(device, {instance})});

    CHECK_EQ(world.getBounds(), instance.getBounds());
    CHECK_EQ(world.getBounds(), group.getBounds());

    auto future = render(
        device,
        {
            .framebuffer = framebuffer,
            .renderer = renderer,
            .camera = camera,
            .world = world,
        });

    auto duration = future.waitAndGetDuration();

    CHECK(duration > 0);
    CHECK(future.getProgress() == 1);

    auto data = framebuffer.map(FramebufferChannel::Color);

    rkcommon::utility::writePPM("test.ppm", width, height, data.as<std::uint32_t>());

    CHECK(error.empty());
}
