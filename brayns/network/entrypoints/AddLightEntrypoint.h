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

#include <brayns/network/adapters/LightAdapter.h>
#include <brayns/network/entrypoint/Entrypoint.h>

namespace brayns
{
class AddLightHelper
{
public:
    static void load(PluginAPI &api, LightPtr light, const NetworkRequest &request);
};

template<typename T>
class AddLightEntrypoint : public BaseEntrypoint
{
public:
    using Ptr = std::shared_ptr<T>;

    virtual JsonSchema getParamsSchema() const override
    {
        return Json::getSchema<T>();
    }

    virtual JsonSchema getResultSchema() const override
    {
        return Json::getSchema<size_t>();
    }

    virtual void onRequest(const NetworkRequest &request) override
    {
        auto params = request.getParams();
        auto light = Json::deserialize<Ptr>(params);
        auto &api = getApi();
        AddLightHelper::load(api, std::move(light), request);
    }
};

class AddLightDirectionalEntrypoint : public AddLightEntrypoint<DirectionalLight>
{
public:
    virtual std::string getName() const override;
    virtual std::string getDescription() const override;
};

class AddLightSphereEntrypoint : public AddLightEntrypoint<SphereLight>
{
public:
    virtual std::string getName() const override;
    virtual std::string getDescription() const override;
};

class AddLightQuadEntrypoint : public AddLightEntrypoint<QuadLight>
{
public:
    virtual std::string getName() const override;
    virtual std::string getDescription() const override;
};

class AddLightSpotEntrypoint : public AddLightEntrypoint<SpotLight>
{
public:
    virtual std::string getName() const override;
    virtual std::string getDescription() const override;
};

class AddLightAmbientEntrypoint : public AddLightEntrypoint<AmbientLight>
{
public:
    virtual std::string getName() const override;
    virtual std::string getDescription() const override;
};
} // namespace brayns