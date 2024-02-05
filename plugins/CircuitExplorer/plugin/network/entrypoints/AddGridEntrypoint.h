/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#pragma once

#include <brayns/common/Log.h>

#include <brayns/network/entrypoint/Entrypoint.h>

#include <plugin/api/MaterialUtils.h>
#include <plugin/network/messages/AddGridMessage.h>

class GridModel
{
public:
    static void add(brayns::Scene& scene, const AddGridMessage& params)
    {
        // Create grid model
        auto model = scene.createModel();

        // Grid colors
        const brayns::Vector3f red = {1, 0, 0};
        const brayns::Vector3f green = {0, 1, 0};
        const brayns::Vector3f blue = {0, 0, 1};
        const brayns::Vector3f grey = {0.5, 0.5, 0.5};

        auto matId = CircuitExplorerMaterial::createUnlit(*model, grey);

        // Grid lines
        const float m = params.min_value;
        const float M = params.max_value;
        const float s = params.steps;
        const float r = params.radius;
        for (float x = m; x <= M; x += s)
            for (float y = m; y <= M; y += s)
                if (fabs(x) < 0.001f || fabs(y) < 0.001f)
                {
                    model->addCylinder(matId, {{x, y, m}, {x, y, M}, r});
                    model->addCylinder(matId, {{m, x, y}, {M, x, y}, r});
                    model->addCylinder(matId, {{x, m, y}, {x, M, y}, r});
                }

        // X plane
        matId =
            CircuitExplorerMaterial::createUnlit(*model,
                                                 params.use_colors ? red : grey,
                                                 params.plane_opacity);

        auto& tmx = model->getTriangleMeshes()[matId];
        tmx.vertices.push_back({m, 0, m});
        tmx.vertices.push_back({M, 0, m});
        tmx.vertices.push_back({M, 0, M});
        tmx.vertices.push_back({m, 0, M});
        tmx.indices.push_back(brayns::Vector3ui(0, 1, 2));
        tmx.indices.push_back(brayns::Vector3ui(2, 3, 0));

        // Y plane
        matId = CircuitExplorerMaterial::createUnlit(*model,
                                                     params.use_colors ? green
                                                                       : grey,
                                                     params.plane_opacity);
        auto& tmy = model->getTriangleMeshes()[matId];
        tmy.vertices.push_back({m, m, 0});
        tmy.vertices.push_back({M, m, 0});
        tmy.vertices.push_back({M, M, 0});
        tmy.vertices.push_back({m, M, 0});
        tmy.indices.push_back(brayns::Vector3ui(0, 1, 2));
        tmy.indices.push_back(brayns::Vector3ui(2, 3, 0));

        // Z plane
        matId = CircuitExplorerMaterial::createUnlit(*model,
                                                     params.use_colors ? blue
                                                                       : grey,
                                                     params.plane_opacity);
        auto& tmz = model->getTriangleMeshes()[matId];
        tmz.vertices.push_back({0, m, m});
        tmz.vertices.push_back({0, m, M});
        tmz.vertices.push_back({0, M, M});
        tmz.vertices.push_back({0, M, m});
        tmz.indices.push_back(brayns::Vector3ui(0, 1, 2));
        tmz.indices.push_back(brayns::Vector3ui(2, 3, 0));

        // XYZ axis
        if (params.show_axis)
        {
            // Axis properties
            const float l = M;
            const float smallRadius = params.radius * 25.0;
            const float largeRadius = params.radius * 50.0;
            const float l1 = l * 0.89;
            const float l2 = l * 0.90;

            // Axis material properties
            brayns::PropertyMap diffuseProps;
            diffuseProps.add(
                {std::string(MATERIAL_PROPERTY_CAST_USER_DATA), false});
            diffuseProps.add({std::string(MATERIAL_PROPERTY_SHADING_MODE),
                              static_cast<int>(MaterialShadingMode::diffuse)});

            // X
            matId = CircuitExplorerMaterial::create(*model, {1, 0, 0});

            model->addCylinder(matId, {{0, 0, 0}, {l1, 0, 0}, smallRadius});
            model->addCone(matId,
                           {{l1, 0, 0}, {l2, 0, 0}, smallRadius, largeRadius});
            model->addCone(matId, {{l2, 0, 0}, {M, 0, 0}, largeRadius, 0});

            // Y
            matId = CircuitExplorerMaterial::create(*model, {0, 1, 0});

            model->addCylinder(matId, {{0, 0, 0}, {0, l1, 0}, smallRadius});
            model->addCone(matId,
                           {{0, l1, 0}, {0, l2, 0}, smallRadius, largeRadius});
            model->addCone(matId, {{0, l2, 0}, {0, M, 0}, largeRadius, 0});

            // Z
            matId = CircuitExplorerMaterial::create(*model, {0, 0, 1});

            model->addCylinder(matId, {{0, 0, 0}, {0, 0, l1}, smallRadius});
            model->addCone(matId,
                           {{0, 0, l1}, {0, 0, l2}, smallRadius, largeRadius});
            model->addCone(matId, {{0, 0, l2}, {0, 0, M}, largeRadius, 0});

            // Origin
            model->addSphere(0, {{0, 0, 0}, smallRadius});
        }

        // Register model
        scene.addModel(
            std::make_shared<brayns::ModelDescriptor>(std::move(model),
                                                      "Grid"));
    }
};

class AddGridEntrypoint
    : public brayns::Entrypoint<AddGridMessage, brayns::EmptyMessage>
{
public:
    virtual std::string getName() const override { return "add-grid"; }

    virtual std::string getDescription() const override
    {
        return "Add a visual 3D grid to the scene";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        auto& scene = getApi().getScene();
        brayns::Log::info("[CE] Building Grid scene.");
        GridModel::add(scene, params);
        triggerRender();
        request.reply(brayns::EmptyMessage());
    }
};
