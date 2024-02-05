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

#include <messages/CIGetAfferentCellIdsMessage.h>

class AfferentCellRetriever
{
public:
    static std::vector<uint64_t> getAfferentCells(
        const CIGetAfferentCellIdsParams& params)
    {
        std::vector<uint64_t> result;

        // Read config
        brion::BlueConfig config(params.path);
        brain::Circuit circuit(config);

        // Sources stream
        brion::GIDSet sources(params.sources.begin(), params.sources.end());
        auto stream = circuit.getAfferentSynapses(sources);

        // Retreive synapses immediately
        auto future = stream.read(stream.getRemaining());
        auto synapses = future.get();

        // Retreive GIDs and remove duplicates
        auto preGids = synapses.preGIDs();
        std::set<uint32_t> gids = {preGids, preGids + synapses.size()};

        return {gids.begin(), gids.end()};
    }
};

class CIGetAfferentCellIdsEntrypoint
    : public brayns::Entrypoint<CIGetAfferentCellIdsParams,
                                CIGetAfferentCellIdsResult>
{
public:
    virtual std::string getName() const override
    {
        return "ci-get-afferent-cell-ids";
    }

    virtual std::string getDescription() const override
    {
        return "Return a list of afferent synapses cell GIDs from a circuit "
               "and a set of source cells";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        CIGetAfferentCellIdsResult result;
        result.ids = AfferentCellRetriever::getAfferentCells(params);
        request.reply(result);
    }
};