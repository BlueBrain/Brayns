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

    createPerspectiveCamera(device);
    createOrthographicCamera(device);
    createPanoramicCamera(device);

    createToneMapper(device);
    createFramebuffer(device, {{100, 100}});

    auto volumeData = RegularVolumeData{
        .value = std::vector<char>(10 * 10 * 10 * sizeof(float), '\0'),
        .type = VoxelType::F32,
        .size = Size3{10, 10, 10},
    };
    auto volume = createRegularVolume(device, volumeData);

    auto transferFunctionSettings = LinearTransferFunctionSettings{
        .scalarRange = {0, 1},
        .colors = {Color4{1, 0, 0, 1}, Color4{0, 0, 1, 1}},
    };
    auto transferFunction = createLinearTransferFunction(device, transferFunctionSettings);

    createVolumetricModel(device, volume, transferFunction);

    auto triangleMeshSettings = MeshSettings{
        .positions = {{0, 0, 0}, {1, 1, 1}, {2, 2, 2}},
    };

    createTriangleMesh(device, triangleMeshSettings);

    auto quadMeshSettings = MeshSettings{
        .positions = {{0, 0, 0}, {1, 1, 1}, {2, 2, 2}, {3, 3, 3}},
    };
    createQuadMesh(device, quadMeshSettings);

    auto spheres = std::vector<Vector4>{{0, 0, 0, 1}, {1, 1, 1, 1}};

    auto sphereSettings = SphereSettings{spheres};
    auto geometry = createSpheres(device, sphereSettings);

    createDiscs(device, sphereSettings);

    auto curveSettings = CurveSettings{spheres, {0}};
    createCylinders(device, curveSettings);
    createCurve(device, curveSettings);

    auto normals = std::vector<Vector3>{{1, 2, 3}, {4, 5, 6}};
    createRibbon(device, curveSettings, normals);

    auto boxSettings = BoxSettings{{{{0, 0, 0}, {1, 1, 1}}}};
    createBoxes(device, boxSettings);

    auto planeSettings = PlaneSettings{{{1, 2, 3, 4}}};
    createPlanes(device, planeSettings);

    auto isosurfaceSettings = IsosurfaceSettings{{1}};
    createIsosurfaces(device, volume, isosurfaceSettings);

    auto textureData = TextureData2D{
        .value = std::vector<char>(10 * 10 * sizeof(float), '\0'),
        .format = TextureFormat::Rgba32F,
        .size = {10, 10},
    };
    auto texture2D = createTexture2D(device, textureData);

    createVolumeTexture(device, {volume, transferFunction});

    createDistantLight(device);
    createSphereLight(device);
    createSpotLight(device);
    createQuadLight(device);
    createCylinderLight(device);
    createHdriLight(device, texture2D);
    createAmbientLight(device);
    createSunSkyLight(device);

    auto aoMaterial = createAoMaterial(device);
    auto scivisMaterial = createScivisMaterial(device);
    auto principled = createPrincipledMaterial(device);

    auto aoSettings = RendererSettings{{aoMaterial}};
    createAoRenderer(device, aoSettings);

    auto scivisSettings = RendererSettings{{scivisMaterial}};
    createScivisRenderer(device, scivisSettings);

    auto pathTracerSettings = RendererSettings{{principled}};
    createPathTracer(device, pathTracerSettings);

    auto modelSettings = GeometricModelSettings{.materials = {RendererIndex(0)}};
    auto model = createGeometricModel(device, geometry, modelSettings);

    auto groupSettings = GroupSettings{.geometries = {model}};
    auto group = createGroup(device, groupSettings);

    auto instance = createInstance(device, group);

    auto worldSettings = WorldSettings{.instances = {instance}};
    createWorld(device, worldSettings);

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

    auto toneMapper = createToneMapper(device);

    auto framebufferSettings = FramebufferSettings{
        .resolution = {std::size_t(width), std::size_t(height)},
        .format = FramebufferFormat::Srgba8,
        .channels = {FramebufferChannel::Color},
        .operations = std::vector<ImageOperation>{toneMapper},
    };
    auto framebuffer = createFramebuffer(device, framebufferSettings);

    auto material = createScivisMaterial(device);

    auto rendererSettings = RendererSettings{.materials = {material}};
    auto renderer = createScivisRenderer(device, rendererSettings);

    auto camera = createPerspectiveCamera(device, {}, {.fovy = 45.0F, .aspect = float(width) / float(height)});
    // auto camera = createOrthographicCamera(device, {}, {.height = 3, .aspect = float(width) / float(height)});

    auto sphereSettings = SphereSettings{{{0, 0, 3, 0.25F}, {1, 0, 3, 0.25F}, {1, 1, 3, 0.25F}}};
    auto spheres = createSpheres(device, sphereSettings);

    auto modelSettings = GeometricModelSettings{
        .materials = {RendererIndex(0)},
        .colors = std::vector<Color4>{{1, 0, 0, 1}, {0, 0, 1, 1}, {0, 1, 0, 1}},
    };

    auto model = createGeometricModel(device, spheres, modelSettings);

    auto light = createAmbientLight(device);

    auto groupSettings = GroupSettings{
        .geometries = {model},
        .lights = {light},
    };
    auto group = createGroup(device, groupSettings);

    auto instance = createInstance(device, {group});

    auto worldSettings = WorldSettings{{instance}};
    auto world = createWorld(device, worldSettings);

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
