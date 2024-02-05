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

#include <set>

#include <brain/brain.h>
#include <brion/brion.h>

#include <brayns/network/entrypoint/Entrypoint.h>

#include <messages/CIGetEfferentCellIdsMessage.h>

class EfferentCellRetriever
{
public:
    static std::vector<uint64_t> getEfferentCells(
        const CIGetEfferentCellIdsParams& params)
    {
        std::vector<uint64_t> result;

        // Read config
        brion::BlueConfig config(params.path);
        brain::Circuit circuit(config);

        // Sources stream
        brion::GIDSet sources(params.sources.begin(), params.sources.end());
        auto stream = circuit.getEfferentSynapses(sources);

        // Retreive synapses immediately
        auto future = stream.read(stream.getRemaining());
        auto synapses = future.get();

        // Retreive GIDs and remove duplicates
        auto postGids = synapses.postGIDs();
        std::set<uint32_t> gids = {postGids, postGids + synapses.size()};

        return {gids.begin(), gids.end()};
    }
};

class CIGetEfferentCellIdsEntrypoint
    : public brayns::Entrypoint<CIGetEfferentCellIdsParams,
                                CIGetEfferentCellIdsResult>
{
public:
    virtual std::string getName() const override
    {
        return "ci-get-efferent-cell-ids";
    }

    virtual std::string getDescription() const override
    {
        return "Return a list of efferent synapses cell GIDs from a circuit "
               "and a set of source cells";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        CIGetEfferentCellIdsResult result;
        result.ids = EfferentCellRetriever::getEfferentCells(params);
        request.reply(result);
    }
};