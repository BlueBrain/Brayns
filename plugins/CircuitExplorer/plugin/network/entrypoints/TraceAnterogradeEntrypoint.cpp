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

TraceAnterogradeEntrypoint::TraceAnterogradeEntrypoint(CircuitColorManager &manager)
    : _manager(manager)
{
}

std::string TraceAnterogradeEntrypoint::getName() const
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
        throw brayns::EntrypointException("No input cell GIDs specified");
    }

    // Extract API data
    auto modelId = params.model_id;
    auto &scene = getApi().getEngine().getScene();
    auto &model = brayns::ExtractModel::fromId(scene, modelId);

    // Retreive cell mapping
    if (!_manager.handlerExists(model))
    {
        throw brayns::EntrypointException(
            "There given model ID does not correspond to any existing "
            "circuit model");
    }

    _manager.updateSingleColor(model, params.non_connected_cells_color);

    std::map<uint64_t, brayns::Vector4f> colorMap;
    for (const auto gid : params.cell_gids)
        colorMap[gid] = params.source_cell_color;

    for (const auto gid : params.target_cell_gids)
        colorMap[gid] = params.connected_cells_color;

    _manager.updateColorsById(model, colorMap);

    scene.markModified();
    getApi().triggerRender();

    request.reply(brayns::EmptyMessage());
}
