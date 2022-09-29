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

#include <brayns/engine/components/LoadInfo.h>
#include <brayns/engine/components/Metadata.h>
#include <brayns/network/common/ExtractModel.h>
#include <brayns/network/entrypoint/Entrypoint.h>
#include <brayns/utils/string/StringSplitter.h>

#include <messages/CIGetCellIdsFromModelMessage.h>
#include <messages/CIGetCellIdsMessage.h>

#include "CIGetCellIdsEntrypoint.h"

class ModelCellIdsRetriever
{
public:
    static CIGetCellIdsResult getCellIds(brayns::Scene &scene, uint32_t modelId)
    {
        auto &instance = brayns::ExtractModel::fromId(scene, modelId);
        auto &model = instance.getModel();
        auto &components = model.getComponents();

        auto loadInfo = components.find<brayns::LoadInfo>();
        if (!loadInfo || loadInfo->source != brayns::LoadInfo::LoadSource::FromFile)
        {
            throw brayns::JsonRpcException("The model was not loaded through a file");
        }

        CIGetCellIdsParams params;
        params.path = loadInfo->path;

        auto metadata = components.find<brayns::Metadata>();
        if (!metadata)
        {
            return CellIdsRetriever::getCellIds(params);
        }

        auto it = metadata->find("targets");
        if (it == metadata->end())
        {
            return CellIdsRetriever::getCellIds(params);
        }

        auto &target = it->second;
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
