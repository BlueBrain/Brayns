/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
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

#include <plugin/api/ShapeMaterial.h>

#include <plugin/messages/AddCylinderMessage.h>
#include <plugin/messages/AddShapeMessage.h>

class CylinderModel
{
public:
    static size_t add(brayns::Scene& scene, const AddCylinderMessage& params)
    {
        // Create cylinder model
        auto model = scene.createModel();

        // Create cylinder material instance
        ShapeMaterialInfo info;
        info.id = 1;
        info.color = params.color;
        info.opacity = params.color.a;
        ShapeMaterial::create(*model, info);

        // Extract cylinder info
        auto& center = params.center;
        auto& up = params.up;
        auto radius = params.radius;

        // Add geometry
        model->addCylinder(info.id, {center, up, radius});

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
        brayns::Log::info("Building Cylinder model.\n");
        auto id = CylinderModel::add(scene, params);
        scene.markModified();
        triggerRender();
        request.reply({id});
    }
};