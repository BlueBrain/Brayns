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

#include <brain/brain.h>
#include <brion/brion.h>

#include <brayns/network/entrypoint/Entrypoint.h>

#include <messages/CIGetProjectionEfferentCellIdsMessage.h>

class ProjectionEfferentCellRetriever
{
public:
    static std::vector<uint64_t> getEfferentCells(
        const CIGetProjectionEfferentCellIdsParams& params)
    {
        // Read config
        brion::BlueConfig config(params.path);

        // Check projection exists
        auto& projection = params.projection;
        auto section = brion::CONFIGSECTION_PROJECTION;
        auto path = config.get(section, params.projection, "Path");
        if (path.empty())
        {
            throw brayns::EntrypointException(1, "Projection not found");
        }

        // Sources
        brain::Circuit circuit(config);
        brion::GIDSet sources(params.sources.begin(), params.sources.end());
        auto gids = circuit.getProjectedEfferentGIDs(sources, projection);

        // Remove duplicates
        std::set<uint32_t> uniqueGids(gids.begin(), gids.end());

        return {uniqueGids.begin(), uniqueGids.end()};
    }
};

class CIGetProjectionEfferentCellIdsEntrypoint
    : public brayns::Entrypoint<CIGetProjectionEfferentCellIdsParams,
                                CIGetProjectionEfferentCellIdsResult>
{
public:
    virtual std::string getName() const override
    {
        return "ci-get-projection-efferent-cell-ids";
    }

    virtual std::string getDescription() const override
    {
        return "Return a list of efferent projected synapses cell GIDs from a "
               "circuit and a set of source cells";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        CIGetProjectionEfferentCellIdsResult result;
        result.ids = ProjectionEfferentCellRetriever::getEfferentCells(params);
        request.reply(result);
    }
};