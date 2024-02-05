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

#include <messages/CIGetCellIdsMessage.h>

class CellIdsRetriever
{
public:
    static CIGetCellIdsResult getCellIds(const CIGetCellIdsParams& params)
    {
        // Result
        CIGetCellIdsResult result;

        // Load data
        const brion::BlueConfig config(params.path);
        const brain::Circuit circuit(config);

        // Extract GIDs
        brion::GIDSet gids;
        if (params.targets.empty())
        {
            gids = circuit.getGIDs();
        }
        else
        {
            for (const auto& target : params.targets)
            {
                auto temp = circuit.getGIDs(target);
                gids.insert(temp.begin(), temp.end());
            }
        }

        // Fill result
        result.ids.insert(result.ids.end(), gids.begin(), gids.end());

        // Success
        return result;
    }
};

class CIGetCellIdsEntrypoint
    : public brayns::Entrypoint<CIGetCellIdsParams, CIGetCellIdsResult>
{
public:
    virtual std::string getName() const override { return "ci-get-cell-ids"; }

    virtual std::string getDescription() const override
    {
        return "Return the list of GIDs from a circuit";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        auto result = CellIdsRetriever::getCellIds(params);
        request.reply(result);
    }
};