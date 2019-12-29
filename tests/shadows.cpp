/* Copyright (c) 2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Jonas Karlsson <jonas.karlsson@epfl.ch>
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
#include <tests/paths.h>

#include <brayns/common/light/Light.h>
#include <brayns/common/types.h>
#include <brayns/engineapi/Camera.h>
#include <brayns/engineapi/Engine.h>
#include <brayns/engineapi/FrameBuffer.h>
#include <brayns/engineapi/Material.h>
#include <brayns/engineapi/Model.h>
#include <brayns/engineapi/Renderer.h>
#include <brayns/engineapi/Scene.h>
#include <brayns/parameters/ParametersManager.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "PDiffHelpers.h"

TEST_CASE("cylinders")
{
    std::vector<const char*> argv = {
        {"shadows", "--disable-accumulation", "--window-size", "1600", "900",
         "--plugin", "braynsCircuitViewer", "--no-head-light"}};

    brayns::Brayns brayns(argv.size(), argv.data());
    brayns.getParametersManager().getRenderingParameters().setCurrentRenderer(
        "advanced_simulation");
    brayns.commit();
    auto& scene = brayns.getEngine().getScene();

    auto model = scene.createModel();

    const brayns::Vector3f WHITE = {1.f, 1.f, 1.f};
    const brayns::Vector3f RED = {1.f, 0.f, 0.f};
    const brayns::Vector3f GREEN = {0.f, 1.f, 0.f};

    constexpr size_t materialIdRed = 0;
    auto materialRed = model->createMaterial(materialIdRed, "Cylinders Red");
    materialRed->setDiffuseColor(RED);
    materialRed->setSpecularColor(RED);
    materialRed->setSpecularExponent(10.f);

    constexpr size_t materialIdGreen = 1;
    auto materialGreen =
        model->createMaterial(materialIdGreen, "Cylinders Green");
    materialGreen->setDiffuseColor(GREEN);
    materialGreen->setSpecularColor(GREEN);
    materialGreen->setSpecularExponent(10.f);
    materialGreen->setOpacity(0.25f);

    constexpr float row_stride = 0.2f;
    constexpr float thickness = 0.05f;
    constexpr float height = 0.35f;
    constexpr size_t num_cols = 7;

    for (size_t col = 0; col < num_cols; ++col)
    {
        const bool odd = col % 2;
        const size_t num_rows = odd ? num_cols - 1 : num_cols;
        const size_t materialId = odd ? materialIdGreen : materialIdRed;

        for (size_t row = 0; row < num_rows; ++row)
        {
            const float start_x =
                -(static_cast<float>(num_rows - 1) * row_stride * 0.5f);
            const float start_z =
                -(static_cast<float>(num_cols - 1) * row_stride * 0.5f);

            const float x = start_x + row * row_stride;
            const float z = start_z + col * row_stride;
            model->addCylinder(materialId,
                               {{x, 0.0f, z}, {x, height, z}, thickness});
        }
    }

    auto modelDesc = std::make_shared<brayns::ModelDescriptor>(std::move(model),
                                                               "Cylinders");
    scene.addModel(modelDesc);
    scene.getLightManager().clearLights();

    scene.getLightManager().addLight(std::make_unique<brayns::DirectionalLight>(
        brayns::Vector3f(0.f, 0.f, -1.f), 0, WHITE, 1.0f, true));
    scene.commitLights();

    auto& camera = brayns.getEngine().getCamera();

    const double dist = 1.5;
    const double alpha = -std::atan(2 * height / dist);
    const brayns::Quaterniond orientation(
        glm::angleAxis(alpha, brayns::Vector3d(1.0, 0.0, 0.0)));
    camera.setOrientation(orientation);
    camera.setPosition(brayns::Vector3f(0.f, 2 * height, dist));

    auto& renderer = brayns.getEngine().getRenderer();
    renderer.updateProperty("shadows", 1.);

    brayns.commitAndRender();
    CHECK(compareTestImage("shadowCylinders.png",
                           brayns.getEngine().getFrameBuffer()));
}
