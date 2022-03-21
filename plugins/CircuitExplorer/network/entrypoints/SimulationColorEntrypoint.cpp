/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include "SimulationColorEntrypoint.h"

#include <brayns/network/common/ExtractModel.h>

#include <plugin/api/MaterialUtils.h>

SimulationColorEntrypoint::SimulationColorEntrypoint(brayns::Scene &scene)
    : _scene(scene)
{
}

std::string SimulationColorEntrypoint::getMethod() const
{
    return "set-simulation-color";
}

std::string SimulationColorEntrypoint::getDescription() const
{
    return "Enables or disables the color of a given Circuit Model by its "
           "simulation values";
}

void SimulationColorEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    auto modelId = params.model_id;
    auto enabled = params.enabled;
    auto &descriptor = brayns::ExtractModel::fromId(_scene, modelId);
    auto &model = descriptor.getModel();
    CircuitExplorerMaterial::setSimulationColorEnabled(model, enabled);
    request.reply(brayns::EmptyMessage());
}
