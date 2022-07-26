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

#pragma once

#include <brayns/utils/string/StringSplitter.h>

#include <brayns/network/common/ExtractModel.h>
#include <brayns/network/entrypoint/Entrypoint.h>

#include <messages/CIGetCellIdsFromModelMessage.h>
#include <messages/CIGetCellIdsMessage.h>

#include "CIGetCellIdsEntrypoint.h"

class ModelCellIdsRetriever
{
public:
    static CIGetCellIdsResult getCellIds(brayns::Scene &scene, uint32_t modelId)
    {
        // Circuit info
        CIGetCellIdsParams params;

        const auto &loadParams = scene.getModelLoadParameters(modelId);
        if (loadParams.type != brayns::ModelLoadParameters::LoadType::FromFile)
        {
            throw brayns::JsonRpcException("Cannot retrieve cell data from blob-loaded models");
        }

        // Path
        params.path = loadParams.path;

        auto &instance = brayns::ExtractModel::fromId(scene, modelId);
        auto &model = instance.getModel();

        // Targets
        auto &metadata = model.getMetaData();
        auto i = metadata.find("Targets");
        if (i != metadata.end())
        {
            auto &target = i->second;
            if (!target.empty())
            {
                if (target.find(',') == std::string::npos)
                {
                    params.targets.push_back(target);
                }
                else
                {
                    params.targets = brayns::StringSplitter::split(target, ',');
                }
            }
        }

        // Get GIDs from circuit
        return CellIdsRetriever::getCellIds(params);
    }
};

class CIGetCellIdsFromModelEntrypoint : public brayns::Entrypoint<CIGetCellIdsFromModelParams, CIGetCellIdsResult>
{
public:
    CIGetCellIdsFromModelEntrypoint(brayns::Scene &scene)
        : _scene(scene)
    {
    }

    virtual std::string getMethod() const override
    {
        return "ci-get-cell-ids-from-model";
    }

    virtual std::string getDescription() const override
    {
        return "Return the list of GIDs from a loaded circuit";
    }

    virtual void onRequest(const Request &request) override
    {
        auto params = request.getParams();
        auto modelId = params.model_id;
        auto result = ModelCellIdsRetriever::getCellIds(_scene, modelId);
        request.reply(result);
    }

private:
    brayns::Scene &_scene;
};
