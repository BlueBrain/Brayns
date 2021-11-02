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

#include <brayns/network/entrypoint/Entrypoint.h>

#include <plugin/network/adapters/MaterialAdapter.h>
#include <plugin/network/messages/GetMaterialMessage.h>

namespace brayns
{
class GetMaterialEntrypoint
    : public Entrypoint<GetMaterialMessage, MaterialProxy>
{
public:
    virtual std::string getName() const override { return "get-material"; }

    virtual std::string getDescription() const override
    {
        return "Retreive the material with given ID in given model";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        auto modelId = params.model_id;
        auto materialId = params.material_id;
        auto& engine = getApi().getEngine();
        auto& scene = engine.getScene();
        MaterialProxy material(scene);
        material.setModelId(modelId);
        material.setMaterialId(materialId);
        request.reply(material);
    }
};

class SetMaterialEntrypoint : public Entrypoint<MaterialProxy, EmptyMessage>
{
public:
    virtual std::string getName() const override { return "set-material"; }

    virtual std::string getDescription() const override
    {
        return "Update the corresponding material with the given properties";
    }

    virtual void onRequest(const Request& request) override
    {
        auto& engine = getApi().getEngine();
        auto& scene = engine.getScene();
        MaterialProxy material(scene);
        request.getParams(material);
        material.commit();
        scene.markModified();
        triggerRender();
        request.reply(EmptyMessage());
    }
};
} // namespace brayns
