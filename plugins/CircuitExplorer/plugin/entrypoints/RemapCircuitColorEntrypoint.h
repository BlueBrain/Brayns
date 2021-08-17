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

#include <brayns/network/entrypoint/Entrypoint.h>

#include <plugin/CircuitExplorerPlugin.h>

#include <plugin/messages/RemapCircuitColorMessage.h>

class RemapCircuitColorEntrypoint
    : public brayns::Entrypoint<RemapCircuitColorParams,
                                RemapCircuitColorResult>
{
public:
    RemapCircuitColorEntrypoint(CircuitExplorerPlugin& plugin)
        : _plugin(&plugin)
    {
    }

    virtual std::string getName() const override
    {
        return "remap-circuit-color";
    }

    virtual std::string getDescription() const override
    {
        return "Remap the circuit colors to the specified scheme";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        auto updated = _remap(params);
        request.reply({updated});
    }

private:
    bool _remap(const RemapCircuitColorParams& params)
    {
        auto modelId = params.model_id;
        auto scheme = params.scheme;
        auto& engine = getApi().getEngine();
        auto& scene = engine.getScene();
        brayns::ExtractModel::fromId(scene, modelId);
        auto mapper = _plugin->getMapperForCircuit(modelId);
        if (!mapper)
        {
            return false;
        }
        auto status = mapper->remapCircuitColors(scheme, scene);
        if (status.error > 0)
        {
            throw brayns::EntrypointException(status.error, status.message);
        }
        scene.markModified();
        engine.triggerRender();
        return status.updated;
    }

    CircuitExplorerPlugin* _plugin;
};