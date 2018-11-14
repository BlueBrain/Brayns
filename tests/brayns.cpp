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
#include <brayns/common/scene/Model.h>
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
    const char* argv[] = {app, "demo", "--synchronous-mode"};
    const int argc = sizeof(argv) / sizeof(char*);
    brayns::Brayns brayns(argc, argv);

    auto& camera = brayns.getEngine().getCamera();
    BOOST_CHECK_EQUAL(camera.getCurrentType(), "perspective");
    BOOST_CHECK_EQUAL(camera.getPosition(), brayns::Vector3d(0.5, 0.5, 1.5));
    BOOST_CHECK_EQUAL(camera.getOrientation(),
                      brayns::Quaterniond(0.0, 0.0, 0.0, 1.0));

    auto& manipulator = brayns.getCameraManipulator();
    BOOST_CHECK(dynamic_cast<brayns::InspectCenterManipulator*>(&manipulator));

    auto& fb = brayns.getEngine().getFrameBuffer();
    BOOST_CHECK(!fb.getColorBuffer());
    BOOST_CHECK_EQUAL(fb.getColorDepth(), 4);
    BOOST_CHECK(!fb.getDepthBuffer());
    BOOST_CHECK_EQUAL(fb.getSize(), brayns::Vector2i(800, 600));

    auto& pm = brayns.getParametersManager();
    const auto& appParams = pm.getApplicationParameters();
    BOOST_CHECK(appParams.getEngine() == brayns::EngineType::ospray);
    BOOST_CHECK(appParams.getOsprayModules().empty());
    BOOST_CHECK_EQUAL(appParams.getWindowSize(), brayns::Vector2ui(800, 600));
    BOOST_CHECK(!appParams.isBenchmarking());
    BOOST_CHECK_EQUAL(appParams.getJpegCompression(), 90);
    BOOST_CHECK_EQUAL(appParams.getImageStreamFPS(), 60);

    const auto& renderParams = pm.getRenderingParameters();
    BOOST_CHECK_EQUAL(renderParams.getCameraType(), "perspective");
    BOOST_CHECK_EQUAL(renderParams.getCurrentRenderer(), "basic");
    BOOST_CHECK_EQUAL(renderParams.getRenderers().size(), 8);
    BOOST_CHECK_EQUAL(renderParams.getSamplesPerPixel(), 1);
    BOOST_CHECK_EQUAL(renderParams.getBackgroundColor(),
                      brayns::Vector3d(0, 0, 0));

    const auto& geomParams = pm.getGeometryParameters();
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
    BOOST_CHECK_EQUAL(geomParams.getCircuitStartSimulationTime(), 0.f);
    BOOST_CHECK_EQUAL(geomParams.getCircuitEndSimulationTime(),
                      std::numeric_limits<float>::max());
    BOOST_CHECK_EQUAL(geomParams.getCircuitSimulationValuesRange().x(),
                      std::numeric_limits<double>::max());
    BOOST_CHECK_EQUAL(geomParams.getCircuitSimulationValuesRange().y(),
                      std::numeric_limits<double>::min());

    const auto& animParams = pm.getAnimationParameters();
    BOOST_CHECK_EQUAL(animParams.getFrame(), 0);

    const auto& volumeParams = pm.getVolumeParameters();
    BOOST_CHECK_EQUAL(volumeParams.getDimensions(), brayns::Vector3ui(0, 0, 0));
    BOOST_CHECK_EQUAL(volumeParams.getElementSpacing(),
                      brayns::Vector3d(1., 1., 1.));
    BOOST_CHECK_EQUAL(volumeParams.getOffset(), brayns::Vector3d(0., 0., 0.));

    const auto& sceneParams = pm.getSceneParameters();
    BOOST_CHECK_EQUAL(sceneParams.getEnvironmentMap(), "");

    auto& scene = brayns.getEngine().getScene();
    brayns::Boxd defaultBoundingBox;
    defaultBoundingBox.merge(brayns::Vector3d(0, 0, 0));
    defaultBoundingBox.merge(brayns::Vector3d(1, 1, 1));
    BOOST_CHECK_EQUAL(scene.getBounds(), defaultBoundingBox);
    BOOST_CHECK(geomParams.getMemoryMode() == brayns::MemoryMode::shared);
}

BOOST_AUTO_TEST_CASE(bvh_type)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();
    const char* app = testSuite.argv[0];
    const char* argv[] = {
        app,       "demo", "--default-bvh-flag", "robust", "--default-bvh-flag",
        "compact",
    };
    const int argc = sizeof(argv) / sizeof(char*);
    brayns::Brayns brayns(argc, argv);

    auto model = brayns.getEngine().getScene().getModel(0);
    const auto& bvhFlags = model->getModel().getBVHFlags();

    BOOST_CHECK(bvhFlags.count(brayns::BVHFlag::robust) > 0);
    BOOST_CHECK(bvhFlags.count(brayns::BVHFlag::compact) > 0);
}
