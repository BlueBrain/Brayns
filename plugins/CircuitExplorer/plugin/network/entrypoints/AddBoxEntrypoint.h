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
#include <brayns/common/geometry/TriangleMesh.h>

#include <brayns/network/entrypoint/Entrypoint.h>

#include <plugin/api/MaterialUtils.h>
#include <plugin/network/messages/AddBoxMessage.h>
#include <plugin/network/messages/AddShapeMessage.h>

class BoxModel
{
public:
    static size_t add(brayns::Scene& scene, const AddBoxMessage& params)
    {
        // Create box model
        auto model = scene.createModel();

        // Create box material instance
        const auto matId =
            CircuitExplorerMaterial::create(*model,
                                            brayns::Vector3f(params.color),
                                            params.color.a);

        // Extract box info
        auto& minCorner = params.min_corner;
        auto& maxCorner = params.max_corner;

        // Create box mesh
        auto mesh = brayns::createBox(minCorner, maxCorner);
        auto& meshes = model->getTriangleMeshes();
        meshes[matId] = mesh;
        model->markInstancesDirty();

        // Box model name
        size_t count = scene.getNumModels();
        auto name = params.name;
        if (name.empty())
        {
            name = "box_" + std::to_string(count);
        }

        // Register box model and return its ID
        return scene.addModel(
            std::make_shared<brayns::ModelDescriptor>(std::move(model), name));
    }
};

class AddBoxEntrypoint
    : public brayns::Entrypoint<AddBoxMessage, AddShapeMessage>
{
public:
    virtual std::string getName() const override { return "add-box"; }

    virtual std::string getDescription() const override
    {
        return "Add a visual 3D box to the scene";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        auto& scene = getApi().getScene();
        brayns::Log::info("[CE] Building Box model.");
        auto id = BoxModel::add(scene, params);
        scene.markModified();
        triggerRender();
        request.reply({id});
    }
};
