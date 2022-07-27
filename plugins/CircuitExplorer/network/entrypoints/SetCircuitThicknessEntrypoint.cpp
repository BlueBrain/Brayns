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

#include "SetCircuitThicknessEntrypoint.h"

#include <brayns/network/common/ExtractModel.h>

#include <components/MorphologyCircuitComponent.h>

namespace
{
class CircuitThicknessModifier
{
public:
    static void setCircuitThickness(brayns::Scene &scene, const SetCircuitThicknessMessage &params)
    {
        // Extract params
        auto modelId = params.model_id;
        auto radiusMultiplier = params.radius_multiplier;

        if (radiusMultiplier == 1.f)
        {
            return;
        }

        // Extract model
        auto &instance = brayns::ExtractModel::fromId(scene, modelId);
        auto &model = instance.getModel();

        MorphologyCircuitComponent *circuit = nullptr;
        try
        {
            auto &circuitComponent = model.getComponent<MorphologyCircuitComponent>();
            circuit = &circuitComponent;
        }
        catch (const std::exception &)
        {
            throw brayns::JsonRpcException("The model is not a morphological neuron circuit");
        }

        circuit->changeThickness(radiusMultiplier);

        // We have modified the thickness, lets recompute the bounds
        instance.computeBounds();
        scene.computeBounds();
    }
};
} // namespace

SetCircuitThicknessEntrypoint::SetCircuitThicknessEntrypoint(brayns::Scene &scene)
    : _scene(scene)
{
}

std::string SetCircuitThicknessEntrypoint::getMethod() const
{
    return "set-circuit-thickness";
}

std::string SetCircuitThicknessEntrypoint::getDescription() const
{
    return "Modify the geometry radiuses (spheres, cones, cylinders and SDF geometries)";
}

void SetCircuitThicknessEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    CircuitThicknessModifier::setCircuitThickness(_scene, params);
    request.reply(brayns::EmptyMessage());
}
