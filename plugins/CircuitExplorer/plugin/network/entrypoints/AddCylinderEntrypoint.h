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
#include <brayns/common/geometry/SDFGeometry.h>

#include <brayns/network/entrypoint/Entrypoint.h>

#include <plugin/api/MaterialUtils.h>
#include <plugin/network/messages/AddCylinderMessage.h>
#include <plugin/network/messages/AddShapeMessage.h>

class CylinderModel
{
public:
    static size_t add(brayns::Scene& scene, const AddCylinderMessage& params)
    {
        // Create cylinder model
        auto model = scene.createModel();

        // Create cylinder material instance
        const auto matId =
            CircuitExplorerMaterial::create(*model,
                                            brayns::Vector3f(params.color),
                                            params.color.a);

        // Extract cylinder info
        auto& center = params.center;
        auto& up = params.up;
        auto radius = static_cast<float>(params.radius);

        // Add geometry
        model->addCylinder(matId, {center, up, radius});

        // Cylinder model name
        size_t count = scene.getNumModels();
        auto name = params.name;
        if (name.empty())
        {
            name = "cylinder_" + std::to_string(count);
        }

        // Register cylinder model and return its ID
        return scene.addModel(
            std::make_shared<brayns::ModelDescriptor>(std::move(model), name));
    }
};

class AddCylinderEntrypoint
    : public brayns::Entrypoint<AddCylinderMessage, AddShapeMessage>
{
public:
    virtual std::string getName() const override { return "add-cylinder"; }

    virtual std::string getDescription() const override
    {
        return "Add a visual 3D cylinder to the scene";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        auto& scene = getApi().getScene();
        brayns::Log::info("[CE] Building Cylinder model.\n");
        auto id = CylinderModel::add(scene, params);
        scene.markModified();
        triggerRender();
        request.reply({id});
    }
};
