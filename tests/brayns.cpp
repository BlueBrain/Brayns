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

#include <brayns/engineapi/Camera.h>
#include <brayns/engineapi/Engine.h>
#include <brayns/engineapi/FrameBuffer.h>
#include <brayns/engineapi/Model.h>
#include <brayns/engineapi/Scene.h>
#include <brayns/manipulators/InspectCenterManipulator.h>
#include <brayns/parameters/ParametersManager.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

TEST_CASE("simple_construction")
{
    const char* argv[] = {"brayns"};
    CHECK_NOTHROW(brayns::Brayns(1, argv));
}

TEST_CASE("defaults")
{
    const char* argv[] = {"brayns", "demo"};
    const int argc = sizeof(argv) / sizeof(char*);
    brayns::Brayns brayns(argc, argv);

    auto& camera = brayns.getEngine().getCamera();
    CHECK_EQ(camera.getCurrentType(), "perspective");
    CHECK_EQ(camera.getPosition(), brayns::Vector3d(0.5, 0.5, 1.5));
    CHECK_EQ(camera.getOrientation(), brayns::Quaterniond(1, 0, 0, 0));

    auto& manipulator = brayns.getCameraManipulator();
    CHECK(dynamic_cast<brayns::InspectCenterManipulator*>(&manipulator));

    auto& fb = brayns.getEngine().getFrameBuffer();
    CHECK(!fb.getColorBuffer());
    CHECK_EQ(fb.getColorDepth(), 4);
    CHECK(!fb.getDepthBuffer());
    CHECK_EQ(fb.getSize(), brayns::Vector2ui(800, 600));

    auto& pm = brayns.getParametersManager();
    const auto& appParams = pm.getApplicationParameters();
    CHECK(appParams.getEngine() == "ospray");
    CHECK(appParams.getOsprayModules().empty());
    CHECK_EQ(appParams.getWindowSize(), brayns::Vector2ui(800, 600));
    CHECK(!appParams.isBenchmarking());
    CHECK_EQ(appParams.getJpegCompression(), 90);
    CHECK_EQ(appParams.getImageStreamFPS(), 60);

    const auto& renderParams = pm.getRenderingParameters();
    CHECK_EQ(renderParams.getCurrentCamera(), "perspective");
    CHECK_EQ(renderParams.getCurrentRenderer(), "basic");
    CHECK_EQ(renderParams.getCameras().size(), 5);
    CHECK_EQ(renderParams.getRenderers().size(), 5);
    CHECK_EQ(renderParams.getSamplesPerPixel(), 1);
    CHECK_EQ(renderParams.getBackgroundColor(), brayns::Vector3d(0, 0, 0));

    const auto& geomParams = pm.getGeometryParameters();
    CHECK(geomParams.getColorScheme() == brayns::ProteinColorScheme::none);
    CHECK(geomParams.getGeometryQuality() == brayns::GeometryQuality::high);

    const auto& animParams = pm.getAnimationParameters();
    CHECK_EQ(animParams.getFrame(), 0);

    const auto& volumeParams = pm.getVolumeParameters();
    CHECK_EQ(volumeParams.getDimensions(), brayns::Vector3ui(0, 0, 0));
    CHECK_EQ(volumeParams.getElementSpacing(), brayns::Vector3d(1., 1., 1.));
    CHECK_EQ(volumeParams.getOffset(), brayns::Vector3d(0., 0., 0.));

    auto& scene = brayns.getEngine().getScene();
    brayns::Boxd defaultBoundingBox;
    defaultBoundingBox.merge(brayns::Vector3d(0, 0, 0));
    defaultBoundingBox.merge(brayns::Vector3d(1, 1, 1));
    CHECK_EQ(scene.getBounds(), defaultBoundingBox);
    CHECK(geomParams.getMemoryMode() == brayns::MemoryMode::shared);
}

TEST_CASE("bvh_type")
{
    const char* argv[] = {
        "brayns",  "demo", "--default-bvh-flag", "robust", "--default-bvh-flag",
        "compact",
    };
    const int argc = sizeof(argv) / sizeof(char*);
    brayns::Brayns brayns(argc, argv);

    auto model = brayns.getEngine().getScene().getModel(0);
    const auto& bvhFlags = model->getModel().getBVHFlags();

    CHECK(bvhFlags.count(brayns::BVHFlag::robust) > 0);
    CHECK(bvhFlags.count(brayns::BVHFlag::compact) > 0);
}
