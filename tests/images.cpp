/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <brayns/engine/components/GeometryRendererComponent.h>
#include <brayns/engine/geometry/types/Sphere.h>
#include <brayns/engine/light/types/AmbientLight.h>
#include <brayns/engine/light/types/DirectionalLight.h>

#include <tests/paths.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "helpers/BraynsTestUtils.h"
#include "helpers/ImageValidator.h"

TEST_CASE("render_two_frames_and_compare_they_are_same")
{
    brayns::Brayns brayns;

    BraynsTestUtils::setRenderResolution(brayns, 300, 300);
    BraynsTestUtils::addModel(brayns, BRAYNS_TESTDATA_MODEL_PLY_PATH);
    BraynsTestUtils::addLight(brayns, brayns::Light(brayns::DirectionalLight()));
    BraynsTestUtils::addLight(brayns, brayns::Light(brayns::AmbientLight{0.05f}));
    BraynsTestUtils::adjustPerspectiveView(brayns);

    auto &engine = brayns.getEngine();
    auto &framebuffer = engine.getFramebuffer();

    brayns.commitAndRender();
    auto oldImage = framebuffer.getImage();

    framebuffer.clear();

    brayns.commitAndRender();
    auto newImage = framebuffer.getImage();

    CHECK(ImageValidator::validate(oldImage, newImage));
}

TEST_CASE("render_xyz_and_compare")
{
    brayns::Brayns brayns;

    BraynsTestUtils::setRenderResolution(brayns, 300, 300);
    BraynsTestUtils::addModel(brayns, BRAYNS_TESTDATA_MODEL_XYZ_PATH);
    BraynsTestUtils::addLight(brayns, brayns::Light(brayns::DirectionalLight()));
    BraynsTestUtils::addLight(brayns, brayns::Light(brayns::AmbientLight{0.05f}));
    BraynsTestUtils::adjustPerspectiveView(brayns);
    brayns.commitAndRender();

    auto &engine = brayns.getEngine();
    CHECK(ImageValidator::validate(engine, "testImagesXYZ.png"));

    auto &scene = engine.getScene();
    auto &instance = scene.getModelInstance(0);
    auto &model = instance.getModel();
    auto &component = model.getComponent<brayns::GeometryRendererComponent>();
    auto &geometry = component.getGeometry();
    geometry.forEach([](brayns::Sphere &sphere) { sphere.radius *= 0.5f; });
    brayns.commitAndRender();

    CHECK(ImageValidator::validate(engine, "testImagesXYZSmaller.png"));
}

TEST_CASE("render_protein_and_compare")
{
    brayns::Brayns brayns;

    BraynsTestUtils::setRenderResolution(brayns, 300, 300);
    BraynsTestUtils::addModel(brayns, BRAYNS_TESTDATA_MODEL_PDB_PATH);
    BraynsTestUtils::addLight(brayns, brayns::Light(brayns::DirectionalLight()));
    BraynsTestUtils::addLight(brayns, brayns::Light(brayns::AmbientLight{0.05f}));
    BraynsTestUtils::adjustPerspectiveView(brayns);
    brayns.commitAndRender();

    auto &engine = brayns.getEngine();
    CHECK(ImageValidator::validate(engine, "testImagesPDB.png"));
}

TEST_CASE("render_ply_and_compare")
{
    brayns::Brayns brayns;

    BraynsTestUtils::setRenderResolution(brayns, 300, 300);
    BraynsTestUtils::addModel(brayns, BRAYNS_TESTDATA_MODEL_PLY_PATH);
    BraynsTestUtils::addLight(brayns, brayns::Light(brayns::DirectionalLight()));
    BraynsTestUtils::addLight(brayns, brayns::Light(brayns::AmbientLight{0.05f}));
    BraynsTestUtils::adjustPerspectiveView(brayns);
    brayns.commitAndRender();

    auto &engine = brayns.getEngine();
    CHECK(ImageValidator::validate(engine, "testImagesPLY.png"));
}
