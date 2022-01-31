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

#include "AddSphereEntrypoint.h"

#include <brayns/common/Log.h>

#include <plugin/api/MaterialUtils.h>

namespace
{
class SphereModel
{
public:
    static size_t add(brayns::Scene &scene, const AddSphereMessage &params)
    {
        // Create sphere model
        auto model = scene.createModel();

        // Create sphere material
        const auto matId = CircuitExplorerMaterial::create(*model, brayns::Vector3f(params.color), params.color.a);

        // Add sphere to model
        const auto &center = params.center;
        const auto radius = static_cast<float>(params.radius);
        model->addSphere(matId, {center, radius});

        // Model name
        size_t count = scene.getNumModels();
        auto name = params.name;
        if (name.empty())
        {
            name = "sphere_" + std::to_string(count);
        }

        // Register model and return its ID
        return scene.addModel(std::make_shared<brayns::ModelDescriptor>(std::move(model), name));
    }
};
} // namespace

std::string AddSphereEntrypoint::getName() const
{
    return "add-sphere";
}

std::string AddSphereEntrypoint::getDescription() const
{
    return "Add a visual 3D sphere to the scene";
}

void AddSphereEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    auto &scene = getApi().getScene();
    brayns::Log::info("[CE] Building Sphere model.");
    auto id = SphereModel::add(scene, params);
    scene.markModified();
    triggerRender();
    request.reply({id});
}
