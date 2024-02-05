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

#include <brayns/network/common/ExtractModel.h>
#include <brayns/network/entrypoint/Entrypoint.h>

#include <plugin/network/messages/SetCircuitThicknessMessage.h>

class CircuitThicknessModifier
{
public:
    CircuitThicknessModifier(brayns::PluginAPI& api)
        : _api(&api)
    {
    }

    void setCircuitThickness(const SetCircuitThicknessMessage& params)
    {
        // Extract params
        auto modelId = params.model_id;
        auto radiusMultiplier = params.radius_multiplier;

        // Extract API data
        auto& engine = _api->getEngine();
        auto& scene = engine.getScene();

        // Extract model
        auto& descriptor = brayns::ExtractModel::fromId(scene, modelId);
        auto& model = descriptor.getModel();

        // Spheres
        auto& sphereMap = model.getSpheres();
        for (auto& entry : sphereMap)
        {
            for (auto& sphere : entry.second)
                sphere.radius *= radiusMultiplier;
        }

        // Cones
        auto& conesMap = model.getCones();
        for (auto& entry : conesMap)
        {
            for (auto& cone : entry.second)
            {
                cone.centerRadius *= radiusMultiplier;
                cone.upRadius *= radiusMultiplier;
            }
        }

        // Cylinders
        auto& cylindersMap = model.getCylinders();
        for (auto& entry : cylindersMap)
        {
            for (auto& cylinder : entry.second)
                cylinder.radius *= radiusMultiplier;
        }

        // SDF
        auto& sdfGeometry = model.getSDFGeometryData();
        for (auto& geom : sdfGeometry.geometries)
        {
            geom.r0 *= radiusMultiplier;
            geom.r1 *= radiusMultiplier;
        }

        // commit
        descriptor.markModified();
        scene.markModified();
        engine.triggerRender();
    }

private:
    brayns::PluginAPI* _api;
};

class SetCircuitThicknessEntrypoint
    : public brayns::Entrypoint<SetCircuitThicknessMessage,
                                brayns::EmptyMessage>
{
public:
    virtual std::string getName() const override
    {
        return "set-circuit-thickness";
    }

    virtual std::string getDescription() const override
    {
        return "Modify the geometry radiuses (spheres, cones, cylinders and "
               "SDF geometries)";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        CircuitThicknessModifier modifier(getApi());
        modifier.setCircuitThickness(params);
        request.reply(brayns::EmptyMessage());
    }
};
