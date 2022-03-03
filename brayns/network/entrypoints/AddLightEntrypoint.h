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

#include <brayns/engine/Scene.h>
#include <brayns/network/adapters/LightAdapter.h>
#include <brayns/network/entrypoint/Entrypoint.h>

namespace brayns
{
template<typename T>
class AddLightEntrypoint : public IEntrypoint
{
public:
    AddLightEntrypoint(Scene &scene)
        : _scene(scene)
    {
    }

    virtual JsonSchema getParamsSchema() const override
    {
        JsonSchema baseSchema = Json::getSchema<T>();
        baseSchema.properties.erase("type");
        return baseSchema;
    }

    virtual JsonSchema getResultSchema() const override
    {
        return Json::getSchema<uint32_t>();
    }

    virtual void onRequest(const JsonRpcRequest &request) override
    {
        auto params = request.getParams();
        auto light = Json::deserialize<std::unique_ptr<T>>(params);

        if (!light)
        {
            throw InvalidParamsException("Failed to extract light properties");
        }

        auto lightId = _scene.addLight(std::move(light));
        request.reply(Json::serialize(lightId));
    }

private:
    Scene &_scene;
};

class AddLightAmbientEntrypoint : public AddLightEntrypoint<AmbientLight>
{
public:
    AddLightAmbientEntrypoint(Scene &scene);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class AddLightDirectionalEntrypoint : public AddLightEntrypoint<DirectionalLight>
{
public:
    AddLightDirectionalEntrypoint(Scene &scene);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class AddLightQuadEntrypoint : public AddLightEntrypoint<QuadLight>
{
public:
    AddLightQuadEntrypoint(Scene &scene);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};
} // namespace brayns
