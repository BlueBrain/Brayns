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

#include <brayns/utils/StringUtils.h>

#include <brayns/network/common/ExtractModel.h>
#include <brayns/network/entrypoint/Entrypoint.h>

#include <messages/CIGetCellIdsFromModelMessage.h>
#include <messages/CIGetCellIdsMessage.h>

#include "CIGetCellIdsEntrypoint.h"

class ModelCellIdsRetriever
{
public:
    static CIGetCellIdsResult getCellIds(brayns::ModelDescriptor& model)
    {
        // Circuit info
        CIGetCellIdsParams params;

        // Path
        params.path = model.getPath();

        // Targets
        auto& metadata = model.getMetadata();
        auto i = metadata.find("Targets");
        if (i != metadata.end())
        {
            auto& target = i->second;
            if (!target.empty())
            {
                if (target.find(',') == std::string::npos)
                {
                    params.targets.push_back(target);
                }
                else
                {
                    params.targets = brayns::string_utils::split(target, ',');
                }
            }
        }

        // Get GIDs from circuit
        return CellIdsRetriever::getCellIds(params);
    }
};

class CIGetCellIdsFromModelEntrypoint
    : public brayns::Entrypoint<CIGetCellIdsFromModelParams, CIGetCellIdsResult>
{
public:
    virtual std::string getName() const override
    {
        return "ci-get-cell-ids-from-model";
    }

    virtual std::string getDescription() const override
    {
        return "Return the list of GIDs from a loaded circuit";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        auto modelId = params.model_id;
        auto& model = brayns::ExtractModel::fromId(getApi(), modelId);
        auto result = ModelCellIdsRetriever::getCellIds(model);
        request.reply(result);
    }
};
