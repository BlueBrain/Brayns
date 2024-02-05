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

#include <brayns/network/adapters/LightAdapter.h>
#include <brayns/network/entrypoint/Entrypoint.h>

namespace brayns
{
template <typename T>
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

    virtual void onRequest(const NetworkRequest& request) override
    {
        auto params = request.getParams();
        auto light = Json::deserialize<Ptr>(params);
        if (!light)
        {
            throw EntrypointException("Failed to extract light properties");
        }
        auto& engine = getApi().getEngine();
        auto& scene = engine.getScene();
        auto& lightManager = scene.getLightManager();
        auto id = lightManager.addLight(std::move(light));
        engine.triggerRender();
        request.reply(Json::serialize(id));
    }
};

class AddLightDirectionalEntrypoint
    : public AddLightEntrypoint<DirectionalLight>
{
public:
    virtual std::string getName() const override
    {
        return "add-light-directional";
    }

    virtual std::string getDescription() const override
    {
        return "Add a directional light and return its ID";
    }
};

class AddLightSphereEntrypoint : public AddLightEntrypoint<SphereLight>
{
public:
    virtual std::string getName() const override { return "add-light-sphere"; }

    virtual std::string getDescription() const override
    {
        return "Add a sphere light and return its ID";
    }
};

class AddLightQuadEntrypoint : public AddLightEntrypoint<QuadLight>
{
public:
    virtual std::string getName() const override { return "add-light-quad"; }

    virtual std::string getDescription() const override
    {
        return "Add a quad light and return its ID";
    }
};

class AddLightSpotEntrypoint : public AddLightEntrypoint<SpotLight>
{
public:
    virtual std::string getName() const override { return "add-light-spot"; }

    virtual std::string getDescription() const override
    {
        return "Add a spot light and return its ID";
    }
};

class AddLightAmbientEntrypoint : public AddLightEntrypoint<AmbientLight>
{
public:
    virtual std::string getName() const override { return "add-light-ambient"; }

    virtual std::string getDescription() const override
    {
        return "Add an ambient light and return its ID";
    }
};
} // namespace brayns