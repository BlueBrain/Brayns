/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/geometry/types/Capsule.h>
#include <brayns/engine/geometry/types/Sphere.h>
#include <brayns/network/common/ExtractModel.h>

namespace
{
class CircuitThicknessModifier
{
public:
    static void set(brayns::ModelManager &models, const SetCircuitThicknessMessage &params)
    {
        // Extract params
        auto modelId = params.model_id;
        auto radiusMultiplier = params.radius_multiplier;
        if (radiusMultiplier == 1.f)
        {
            return;
        }

        auto &instance = brayns::ExtractModel::fromId(models, modelId);
        auto &model = instance.getModel();
        auto &components = model.getComponents();

        if (_setThickness(components.get<brayns::Geometries>(), radiusMultiplier))
        {
            instance.computeBounds();
        }
    }

private:
    template<typename T>
    static bool _testValidPrimitive(brayns::Geometries &geometries)
    {
        auto &first = geometries.elements.front();
        return first.as<T>() != nullptr;
    }

    static bool _setThickness(brayns::Geometries &geometries, float multiplier)
    {
        if (_testValidPrimitive<brayns::Capsule>(geometries))
        {
            _setCapsules(geometries, multiplier);
            return true;
        }
        if (_testValidPrimitive<brayns::Sphere>(geometries))
        {
            _setSpheres(geometries, multiplier);
            return true;
        }
        return false;
    }

    static void _setCapsules(brayns::Geometries &geometries, float multiplier)
    {
        geometries.modified = true;
        for (auto &geometry : geometries.elements)
        {
            geometry.forEach(
                [&](brayns::Capsule &capsule)
                {
                    capsule.r0 *= multiplier;
                    capsule.r1 *= multiplier;
                });
        }
    }

    static void _setSpheres(brayns::Geometries &geometries, float multiplier)
    {
        geometries.modified = true;
        for (auto &geometry : geometries.elements)
        {
            geometry.forEach([&](brayns::Sphere &sphere) { sphere.radius *= multiplier; });
        }
    }
};
} // namespace

SetCircuitThicknessEntrypoint::SetCircuitThicknessEntrypoint(brayns::ModelManager &models):
    _models(models)
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
    CircuitThicknessModifier::set(_models, params);
    request.reply(brayns::EmptyJson());
}
