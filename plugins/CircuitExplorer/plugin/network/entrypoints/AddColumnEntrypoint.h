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
#include <plugin/network/messages/AddColumnMessage.h>

class ColumnModel
{
public:
    static void add(brayns::Scene& scene, const AddColumnMessage& params)
    {
        // Create column model
        auto model = scene.createModel();

        const auto matId = CircuitExplorerMaterial::create(*model);

        // Bottom of the column
        const std::vector<brayns::Vector3f> verticesBottom = {
            {-0.25f, -1.0f, -0.5f}, {0.25f, -1.0f, -0.5f},
            {0.5f, -1.0f, -0.25f},  {0.5f, -1.0f, 0.25f},
            {0.5f, -1.0f, -0.25f},  {0.5f, -1.0f, 0.25f},
            {0.25f, -1.0f, 0.5f},   {-0.25f, -1.0f, 0.5f},
            {-0.5f, -1.0f, 0.25f},  {-0.5f, -1.0f, -0.25f}};

        // Top of the column
        const std::vector<brayns::Vector3f> verticesTop = {
            {-0.25f, 1.f, -0.5f}, {0.25f, 1.f, -0.5f}, {0.5f, 1.f, -0.25f},
            {0.5f, 1.f, 0.25f},   {0.5f, 1.f, -0.25f}, {0.5f, 1.f, 0.25f},
            {0.25f, 1.f, 0.5f},   {-0.25f, 1.f, 0.5f}, {-0.5f, 1.f, 0.25f},
            {-0.5f, 1.f, -0.25f}};

        // Extract column info
        const auto r = static_cast<float>(params.radius);

        // Bottom
        for (size_t i = 0; i < verticesBottom.size(); ++i)
        {
            model->addCylinder(matId,
                               {verticesBottom[i],
                                verticesBottom[(i + 1) % verticesBottom.size()],
                                r / 2.f});
            model->addSphere(matId, {verticesBottom[i], r});
        }

        // Top
        for (size_t i = 0; i < verticesTop.size(); ++i)
        {
            model->addCylinder(matId,
                               {verticesTop[i],
                                verticesTop[(i + 1) % verticesTop.size()],
                                r / 2.f});
            model->addSphere(matId, {verticesTop[i], r});
        }

        // Middle
        for (size_t i = 0; i < verticesTop.size(); ++i)
            model->addCylinder(matId,
                               {verticesBottom[i], verticesTop[i], r / 2.f});

        // Register column model
        scene.addModel(
            std::make_shared<brayns::ModelDescriptor>(std::move(model),
                                                      "Column"));
    }
};

class AddColumnEntrypoint
    : public brayns::Entrypoint<AddColumnMessage, brayns::EmptyMessage>
{
public:
    virtual std::string getName() const override { return "add-column"; }

    virtual std::string getDescription() const override
    {
        return "Add a visual 3D column as a cylinder to the scene";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        auto& scene = getApi().getScene();
        brayns::Log::info("[CE] Building Column model.");
        ColumnModel::add(scene, params);
        triggerRender();
        request.reply(brayns::EmptyMessage());
    }
};
