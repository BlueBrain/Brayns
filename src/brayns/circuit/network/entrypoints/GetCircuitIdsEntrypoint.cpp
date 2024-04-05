/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: nadir.romanguerrero@epfl.ch
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

#include "GetCircuitIdsEntrypoint.h"

#include <brayns/core/network/common/ExtractModel.h>

#include <brayns/circuit/components/CircuitIds.h>

GetCircuitIdsEntrypoint::GetCircuitIdsEntrypoint(brayns::ModelManager &models):
    _models(models)
{
}

std::string GetCircuitIdsEntrypoint::getMethod() const
{
    return "get-circuit-ids";
}

std::string GetCircuitIdsEntrypoint::getDescription() const
{
    return "For neuron/astrocyte/vasculature models, return the list of ids loaded";
}

void GetCircuitIdsEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    auto modelId = params.model_id;
    auto &instance = brayns::ExtractModel::fromId(_models, modelId);
    auto &model = instance.getModel();
    auto &components = model.getComponents();

    auto circuitIds = components.find<CircuitIds>();
    if (!circuitIds)
    {
        throw brayns::JsonRpcException("The model does not have a list of Ids");
    }

    request.reply(circuitIds->elements);
}
