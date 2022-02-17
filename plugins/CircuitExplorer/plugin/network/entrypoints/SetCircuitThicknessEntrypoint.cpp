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

        // Extract model
        auto &descriptor = brayns::ExtractModel::fromId(scene, modelId);
        auto &model = descriptor.getModel();

        // Spheres
        auto &sphereMap = model.getSpheres();
        for (auto &entry : sphereMap)
        {
            for (auto &sphere : entry.second)
                sphere.radius *= radiusMultiplier;
        }

        // Cones
        auto &conesMap = model.getCones();
        for (auto &entry : conesMap)
        {
            for (auto &cone : entry.second)
            {
                cone.centerRadius *= radiusMultiplier;
                cone.upRadius *= radiusMultiplier;
            }
        }

        // Cylinders
        auto &cylindersMap = model.getCylinders();
        for (auto &entry : cylindersMap)
        {
            for (auto &cylinder : entry.second)
                cylinder.radius *= radiusMultiplier;
        }

        // SDF
        auto &sdfGeometry = model.getSDFGeometryData();
        for (auto &geom : sdfGeometry.geometries)
        {
            geom.r0 *= radiusMultiplier;
            geom.r1 *= radiusMultiplier;
        }

        // commit
        descriptor.markModified();
        scene.markModified();
    }
};
} // namespace

SetCircuitThicknessEntrypoint::SetCircuitThicknessEntrypoint(brayns::Scene &scene)
    : _scene(scene)
{
}

std::string SetCircuitThicknessEntrypoint::getName() const
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
