/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include "AddPillEntrypoint.h"

#include <brayns/common/Log.h>
#include <brayns/common/geometry/SDFGeometry.h>

#include <plugin/api/MaterialUtils.h>

namespace
{
class PillModel
{
public:
    static size_t add(brayns::Scene &scene, const AddPillMessage &params)
    {
        // Create pill model
        auto model = scene.createModel();

        // Create pill material instance
        const auto matId = CircuitExplorerMaterial::create(*model, brayns::Vector3f(params.color), params.color.a);

        // Extract pill info
        auto type = params.type;
        auto &p1 = params.p1;
        auto &p2 = params.p2;
        auto radius1 = params.radius1;
        auto radius2 = params.radius2;

        // Build geometry
        brayns::SDFGeometry sdf;
        switch (type)
        {
        case PillType::Pill:
            sdf = brayns::createSDFPill(p1, p2, radius1);
            break;
        case PillType::ConePill:
            sdf = brayns::createSDFConePill(p1, p2, radius1, radius2);
            break;
        case PillType::SigmoidPill:
            sdf = brayns::createSDFConePillSigmoid(p1, p2, radius1, radius2);
            break;
        }

        // Add geometry
        model->addSDFGeometry(matId, sdf, {});

        // Pill model name
        size_t count = scene.getNumModels();
        auto name = params.name;
        if (name.empty())
        {
            name = brayns::GetEnumName::of(type) + "_" + std::to_string(count);
        }

        // Register pill model and return its ID
        return scene.addModel(std::make_shared<brayns::ModelDescriptor>(std::move(model), name));
    }
};
} // namespace

AddPillEntrypoint::AddPillEntrypoint(brayns::Scene &scene)
    : _scene(scene)
{
}

std::string AddPillEntrypoint::getMethod() const
{
    return "add-pill";
}

std::string AddPillEntrypoint::getDescription() const
{
    return "Add a visual 3D pill to the scene";
}

void AddPillEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    brayns::Log::info("[CE] Building Pill model.");
    auto id = PillModel::add(_scene, params);
    request.reply({id});
}
