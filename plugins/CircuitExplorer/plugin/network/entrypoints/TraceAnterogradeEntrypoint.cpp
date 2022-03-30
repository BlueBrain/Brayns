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

#include "TraceAnterogradeEntrypoint.h"

#include <brayns/network/common/ExtractModel.h>

#include <plugin/components/CircuitColorComponent.h>

TraceAnterogradeEntrypoint::TraceAnterogradeEntrypoint(brayns::SceneModelManager &modelManager)
    : _modelManager(modelManager)
{
}

std::string TraceAnterogradeEntrypoint::getMethod() const
{
    return "trace-anterograde";
}

std::string TraceAnterogradeEntrypoint::getDescription() const
{
    return "Performs neuronal tracing showing efferent and afferent "
           "synapse relationship between cells (including projections)";
}

void TraceAnterogradeEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();

    // Validation
    if (params.cell_gids.empty())
    {
        throw brayns::JsonRpcException("No input cell GIDs specified");
    }

    // Extract API data
    auto modelId = params.model_id;
    auto &modelInstance = brayns::ExtractModel::fromId(_modelManager, modelId);
    auto &model = modelInstance.getModel();
    auto &colorComponent = model.getComponent<CircuitColorComponent>();
    auto &colorHandler = colorComponent.getColorHandler();

    const auto &srcIDs = params.cell_gids;
    const auto &targetIDs = params.target_cell_gids;
    const auto &baseColor = params.non_connected_cells_color;
    const auto &srcColor = params.source_cell_color;
    const auto &targetColor = params.connected_cells_color;

    colorHandler.updateSingleColor(baseColor);

    std::map<uint64_t, brayns::Vector4f> colorMap;
    for (const auto gid : srcIDs)
    {
        colorMap[gid] = srcColor;
    }

    for (const auto gid : targetIDs)
    {
        colorMap[gid] = targetColor;
    }

    colorHandler.updateColorById(colorMap);

    request.reply(brayns::EmptyMessage());
}
