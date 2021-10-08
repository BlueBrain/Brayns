/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
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

#include <brayns/network/common/ExtractModel.h>
#include <brayns/network/entrypoint/Entrypoint.h>

#include <plugin/io/BrickLoader.h>
#include <plugin/messages/SaveModelToCacheMessage.h>

class SaveModelToCacheEntrypoint
    : public brayns::Entrypoint<SaveModelToCacheMessage, brayns::EmptyMessage>
{
public:
    virtual std::string getName() const override
    {
        return "save-model-to-cache";
    }

    virtual std::string getDescription() const override
    {
        return "Saves given model in a cache file";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        auto modelId = params.model_id;
        auto& path = params.path;
        auto& engine = getApi().getEngine();
        auto& scene = engine.getScene();
        auto& model = brayns::ExtractModel::fromId(scene, modelId);
        BrickLoader loader(scene);
        loader.exportToFile(model, path);
        request.reply(brayns::EmptyMessage());
    }
};