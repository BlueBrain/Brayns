/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "EnableSimulationEntrypoint.h"

#include <brayns/core/engine/components/SimulationInfo.h>
#include <brayns/core/network/common/ExtractModel.h>

namespace brayns
{
EnableSimulationEntrypoint::EnableSimulationEntrypoint(ModelManager &models):
    _models(models)
{
}

std::string EnableSimulationEntrypoint::getMethod() const
{
    return "enable-simulation";
}

std::string EnableSimulationEntrypoint::getDescription() const
{
    return "A switch to enable or disable simulation on a model";
}

void EnableSimulationEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    auto modelId = params.model_id;
    auto &instance = ExtractModel::fromId(_models, modelId);
    auto &model = instance.getModel();
    auto &components = model.getComponents();

    auto simulation = components.find<SimulationInfo>();
    if (!simulation)
    {
        throw JsonRpcException("The model does not have simulation");
    }

    simulation->enabled = params.enabled;
    request.reply(EmptyJson());
}
}
