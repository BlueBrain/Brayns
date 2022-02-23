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

#include <brayns/engine/LightManager.h>

#include <brayns/network/adapters/LightAdapter.h>
#include <brayns/network/entrypoint/IEntrypoint.h>

namespace brayns
{
class AddLightHelper
{
public:
    static void load(LightManager &lights, LightPtr light, const JsonRpcRequest &request);
};

template<typename T>
class AddLightEntrypoint : public IEntrypoint
{
public:
    AddLightEntrypoint(LightManager &lights)
        : _lights(lights)
    {
    }

    virtual JsonSchema getParamsSchema() const override
    {
        return Json::getSchema<T>();
    }

    virtual JsonSchema getResultSchema() const override
    {
        return Json::getSchema<size_t>();
    }

    virtual void onRequest(const JsonRpcRequest &request) override
    {
        auto params = request.getParams();
        auto light = Json::deserialize<std::shared_ptr<T>>(params);
        AddLightHelper::load(_lights, std::move(light), request);
    }

private:
    LightManager &_lights;
};

class AddLightDirectionalEntrypoint : public AddLightEntrypoint<DirectionalLight>
{
public:
    AddLightDirectionalEntrypoint(LightManager &lights);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class AddLightSphereEntrypoint : public AddLightEntrypoint<SphereLight>
{
public:
    AddLightSphereEntrypoint(LightManager &lights);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class AddLightQuadEntrypoint : public AddLightEntrypoint<QuadLight>
{
public:
    AddLightQuadEntrypoint(LightManager &lights);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class AddLightSpotEntrypoint : public AddLightEntrypoint<SpotLight>
{
public:
    AddLightSpotEntrypoint(LightManager &lights);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class AddLightAmbientEntrypoint : public AddLightEntrypoint<AmbientLight>
{
public:
    AddLightAmbientEntrypoint(LightManager &lights);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};
} // namespace brayns
