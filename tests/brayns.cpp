/* Copyright (c) 2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel.Nachbaur@epfl.ch
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

#include <brayns/Brayns.h>

#include <brayns/common/camera/Camera.h>
#include <brayns/common/camera/InspectCenterManipulator.h>
#include <brayns/common/engine/Engine.h>
#include <brayns/common/renderer/FrameBuffer.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/parameters/ParametersManager.h>

#define BOOST_TEST_MODULE brayns
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(simple_construction)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();
    BOOST_CHECK_NO_THROW(
        brayns::Brayns(testSuite.argc,
                       const_cast<const char**>(testSuite.argv)));
}

BOOST_AUTO_TEST_CASE(defaults)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();
    const char* app = testSuite.argv[0];
    const char* argv[] = {app, "demo", "--synchronous-mode", "on"};
    const int argc = sizeof(argv) / sizeof(char*);
    brayns::Brayns brayns(argc, argv);

    auto& camera = brayns.getEngine().getCamera();
    BOOST_CHECK(camera.getType() == brayns::CameraType::default_);
    BOOST_CHECK_EQUAL(camera.getPosition(), brayns::Vector3f(0.5f, 0.5f, 1.5f));
    BOOST_CHECK_EQUAL(camera.getTarget(), brayns::Vector3f(0.5f, 0.5f, 0.5f));
    BOOST_CHECK_EQUAL(camera.getUp(), brayns::Vector3f(0, 1, 0));
    BOOST_CHECK_EQUAL(camera.getAspectRatio(), 4.f / 3.f);
    BOOST_CHECK_EQUAL(camera.getAperture(), 0.f);
    BOOST_CHECK_EQUAL(camera.getFocalLength(), 0.f);

    auto& manipulator = brayns.getCameraManipulator();
    BOOST_CHECK(dynamic_cast<brayns::InspectCenterManipulator*>(&manipulator));

    auto& fb = brayns.getEngine().getFrameBuffer();
    BOOST_CHECK(!fb.getColorBuffer());
    BOOST_CHECK_EQUAL(fb.getColorDepth(), 4);
    BOOST_CHECK(!fb.getDepthBuffer());
    BOOST_CHECK_EQUAL(fb.getSize(), brayns::Vector2i(800, 600));

    auto& pm = brayns.getParametersManager();
    const auto& appParams = pm.getApplicationParameters();
    BOOST_CHECK_EQUAL(appParams.getWindowSize(), brayns::Vector2ui(800, 600));
    BOOST_CHECK(!appParams.isBenchmarking());
    BOOST_CHECK_EQUAL(appParams.getJpegCompression(), 90);
    BOOST_CHECK_EQUAL(appParams.getImageStreamFPS(), 60);

    const auto& renderParams = pm.getRenderingParameters();
    BOOST_CHECK(renderParams.getEngine() == brayns::EngineType::ospray);
    BOOST_CHECK(renderParams.getOsprayModules().empty());
    BOOST_CHECK(renderParams.getCameraType() == brayns::CameraType::default_);
    BOOST_CHECK(renderParams.getStereoMode() == brayns::StereoMode::none);
    BOOST_CHECK(renderParams.getRenderer() == brayns::RendererType::default_);
    BOOST_CHECK_EQUAL(renderParams.getRenderers().size(), 9);
    BOOST_CHECK(!renderParams.getShadowIntensity());
    BOOST_CHECK(!renderParams.getSoftShadows());
    BOOST_CHECK_EQUAL(renderParams.getAmbientOcclusionStrength(), 0.f);
    BOOST_CHECK(renderParams.getShading() == brayns::ShadingType::diffuse);
    BOOST_CHECK_EQUAL(renderParams.getSamplesPerPixel(), 1);
    BOOST_CHECK(!renderParams.getLightEmittingMaterials());
    BOOST_CHECK_EQUAL(renderParams.getBackgroundColor(),
                      brayns::Vector3f(0, 0, 0));
    BOOST_CHECK_EQUAL(renderParams.getDetectionDistance(), 1.f);
    BOOST_CHECK(renderParams.getDetectionOnDifferentMaterial());
    BOOST_CHECK_EQUAL(renderParams.getDetectionNearColor(),
                      brayns::Vector3f(1, 0, 0));
    BOOST_CHECK_EQUAL(renderParams.getDetectionFarColor(),
                      brayns::Vector3f(0, 1, 0));
    BOOST_CHECK(renderParams.getCameraType() == brayns::CameraType::default_);
    BOOST_CHECK_EQUAL(renderParams.getSamplesPerRay(), 128);

    const auto& geomParams = pm.getGeometryParameters();
    BOOST_CHECK_EQUAL(geomParams.getCircuitConfiguration(), "");
    BOOST_CHECK_EQUAL(geomParams.getLoadCacheFile(), "");
    BOOST_CHECK_EQUAL(geomParams.getSaveCacheFile(), "");
    BOOST_CHECK_EQUAL(geomParams.getCircuitTargets(), "");
    BOOST_CHECK_EQUAL(geomParams.getCircuitReport(), "");
    BOOST_CHECK_EQUAL(geomParams.getRadiusMultiplier(), 1.f);
    BOOST_CHECK_EQUAL(geomParams.getRadiusCorrection(), 0.f);
    BOOST_CHECK(geomParams.getColorScheme() == brayns::ColorScheme::none);
    BOOST_CHECK(geomParams.getGeometryQuality() ==
                brayns::GeometryQuality::high);
    BOOST_CHECK_EQUAL(
        brayns::enumsToBitmask(geomParams.getMorphologySectionTypes()),
        brayns::enumsToBitmask(std::vector<brayns::MorphologySectionType>{
            brayns::MorphologySectionType::all}));
    BOOST_CHECK_EQUAL(geomParams.getMorphologyLayout().nbColumns, 0);
    BOOST_CHECK_EQUAL(geomParams.getCircuitStartSimulationTime(), 0.f);
    BOOST_CHECK_EQUAL(geomParams.getCircuitEndSimulationTime(),
                      std::numeric_limits<float>::max());
    BOOST_CHECK_EQUAL(geomParams.getCircuitSimulationValuesRange().x(),
                      std::numeric_limits<float>::max());
    BOOST_CHECK_EQUAL(geomParams.getCircuitSimulationValuesRange().y(),
                      std::numeric_limits<float>::min());

    const auto& animParams = pm.getAnimationParameters();
    BOOST_CHECK_EQUAL(animParams.getFrame(), 0);

    const auto& volumeParams = pm.getVolumeParameters();
    BOOST_CHECK_EQUAL(volumeParams.getDimensions(), brayns::Vector3ui(0, 0, 0));
    BOOST_CHECK_EQUAL(volumeParams.getElementSpacing(),
                      brayns::Vector3f(1.f, 1.f, 1.f));
    BOOST_CHECK_EQUAL(volumeParams.getOffset(),
                      brayns::Vector3f(0.f, 0.f, 0.f));

    const auto& sceneParams = pm.getSceneParameters();
    BOOST_CHECK_EQUAL(sceneParams.getEnvironmentMap(), "");

    auto& scene = brayns.getEngine().getScene();
    brayns::Boxf defaultBoundingBox;
    defaultBoundingBox.merge(brayns::Vector3f(0, 0, 0));
    defaultBoundingBox.merge(brayns::Vector3f(1, 1, 1));
    BOOST_CHECK_EQUAL(scene.getBounds(), defaultBoundingBox);
    BOOST_CHECK(geomParams.getMemoryMode() == brayns::MemoryMode::shared);
}
