/* Copyright (c) 2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Jonas karlsson <jonas.karlsson@epfl.ch>
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

#include <brayns/common/types.h>
#include <brayns/engineapi/Camera.h>
#include <brayns/engineapi/Engine.h>
#include <brayns/engineapi/FrameBuffer.h>
#include <brayns/engineapi/Material.h>
#include <brayns/engineapi/Model.h>
#include <brayns/engineapi/Scene.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "PDiffHelpers.h"

TEST_CASE("streamlines")
{
    const char* argv[] = {"streamlines", "--disable-accumulation",
                          "--window-size", "1600", "900"};
    const int argc = sizeof(argv) / sizeof(char*);

    brayns::Brayns brayns(argc, argv);
    auto& scene = brayns.getEngine().getScene();

    {
        constexpr size_t materialId = 0;
        auto model = scene.createModel();
        const brayns::Vector3f WHITE = {1.f, 1.f, 1.f};

        // Streamline spiral
        auto material = model->createMaterial(materialId, "streamline");
        material->setDiffuseColor(WHITE);
        material->setSpecularColor(WHITE);
        material->setSpecularExponent(10.f);

        for (size_t col = 0; col < 8; ++col)
        {
            for (size_t row = 0; row < 3; ++row)
            {
                brayns::Vector3fs vertices;
                brayns::Vector4fs vertexColors;
                std::vector<float> radii;

                const auto offset =
                    brayns::Vector3f{0.5f * col, 1.f * row, 0.0f};
                const float thicknessStart = 0.03f;
                const float thicknessEnd = 0.005f;

                constexpr size_t numVertices = 70;
                for (size_t i = 0; i < numVertices; ++i)
                {
                    const float t = i / static_cast<float>(numVertices);
                    const auto v =
                        brayns::Vector3f(0.1f * std::cos(i * 0.5f), i * 0.01f,
                                         0.1f * std::sin(i * 0.5f));
                    vertices.push_back(v + offset);
                    radii.push_back((1.f - t) * thicknessStart +
                                    t * thicknessEnd);
                    vertexColors.push_back(
                        brayns::Vector4f(t, std::abs(1.0f - 2.0f * t), 1.0f - t,
                                         1.0f));
                }

                model->addStreamline(materialId,
                                     brayns::Streamline(vertices, vertexColors,
                                                        radii));
            }
        }

        auto modelDesc =
            std::make_shared<brayns::ModelDescriptor>(std::move(model),
                                                      "Streamlines");
        scene.addModel(modelDesc);

        auto position = modelDesc->getModel().getBounds().getCenter();
        position.z += glm::compMax(modelDesc->getModel().getBounds().getSize());

        brayns.getEngine().getCamera().setInitialState(
            position, glm::identity<brayns::Quaterniond>());
    }

    brayns.commitAndRender();
    CHECK(compareTestImage("streamlines.png",
                           brayns.getEngine().getFrameBuffer()));
}
