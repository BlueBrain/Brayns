/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#include <plugin/adapters/MaterialAdapter.h>
#include <plugin/messages/GetMaterialMessage.h>

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
        request.reply(EmptyMessage());
    }
};

class SetMaterialsEntrypoint
    : public Entrypoint<std::vector<MaterialBuffer>, EmptyMessage>
{
public:
    virtual std::string getName() const override { return "set-materials"; }

    virtual std::string getDescription() const override
    {
        return "Update the corresponding materials with the given properties";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        auto& engine = getApi().getEngine();
        auto& scene = engine.getScene();
        for (const auto& buffer : params)
        {
            MaterialProxy material(scene);
            buffer.deserialize(material);
            material.commit();
        }
        scene.markModified();
        request.reply(EmptyMessage());
    }
};

class SetMaterialRangeEntrypoint
    : public Entrypoint<MaterialRangeProxy, EmptyMessage>
{
public:
    virtual std::string getName() const override
    {
        return "set-material-range";
    }

    virtual std::string getDescription() const override
    {
        return "Update the corresponding materials with common properties";
    }

    virtual void onRequest(const Request& request) override
    {
        auto& engine = getApi().getEngine();
        auto& scene = engine.getScene();
        MaterialRangeProxy materialRange(scene);
        request.getParams(materialRange);
        materialRange.commit();
        scene.markModified();
        request.reply(EmptyMessage());
    }
};
} // namespace brayns