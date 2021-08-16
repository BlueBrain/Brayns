/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#include <brayns/common/log.h>

#include <brayns/network/entrypoint/Entrypoint.h>

#include <common/types.h>

#include <plugin/api/ShapeMaterial.h>

#include <plugin/messages/AddShapeMessage.h>
#include <plugin/messages/AddSphereMessage.h>

class SphereModel
{
public:
    static size_t add(brayns::Scene& scene, const AddSphereMessage& params)
    {
        // Create sphere model
        auto model = scene.createModel();

        // Create sphere material
        ShapeMaterialInfo info;
        info.id = 1;
        info.color = params.color;
        info.opacity = params.color[3];
        ShapeMaterial::create(*model, info);

        // Add sphere to model
        auto& center = params.center;
        auto radius = params.radius;
        model->addSphere(info.id, {center, radius});

        // Model name
        size_t numModels = scene.getNumModels();
        auto name = params.name;
        if (name.empty())
        {
            name = "sphere_" + std::to_string(numModels);
        }

        // Register model
        auto id = scene.addModel(
            std::make_shared<brayns::ModelDescriptor>(std::move(model), name));

        // Mark scene modified
        scene.markModified();

        // Return ID of the sphere model
        return id;
    }
};

class AddSphereEntrypoint
    : public brayns::Entrypoint<AddSphereMessage, AddShapeMessage>
{
public:
    virtual std::string getName() const override { return "add-sphere"; }

    virtual std::string getDescription() const override
    {
        return "Add a visual 3D sphere to the scene";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        auto& scene = getApi().getScene();
        BRAYNS_INFO << "Building Sphere model.\n";
        auto id = SphereModel::add(scene, params);
        triggerRender();
        request.reply({id});
    }
};