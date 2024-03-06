/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include <brayns/engine/components/Lights.h>
#include <brayns/engine/json/adapters/LightAdapters.h>
#include <brayns/engine/json/adapters/ModelInstanceAdapter.h>
#include <brayns/engine/scene/ModelManager.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>
#include <brayns/network/entrypoint/Entrypoint.h>

namespace brayns
{
template<typename T>
class AddLightEntrypoint : public Entrypoint<T, ModelInstance>
{
public:
    using Request = typename Entrypoint<T, ModelInstance>::Request;

    explicit AddLightEntrypoint(ModelManager &models):
        _models(models)
    {
    }

    virtual void onRequest(const Request &request) override
    {
        auto model = std::make_shared<Model>("light");

        T data = request.getParams();

        auto &components = model->getComponents();
        auto &lights = components.add<Lights>();
        lights.elements.emplace_back(std::move(data));

        auto &systems = model->getSystems();
        systems.setBoundsSystem<GenericBoundsSystem<Lights>>();

        auto instance = _models.add(std::move(model));
        request.reply(*instance);
    }

private:
    ModelManager &_models;
};

class AddLightAmbientEntrypoint : public AddLightEntrypoint<AmbientLight>
{
public:
    explicit AddLightAmbientEntrypoint(ModelManager &models);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class AddLightDirectionalEntrypoint : public AddLightEntrypoint<DirectionalLight>
{
public:
    explicit AddLightDirectionalEntrypoint(ModelManager &models);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class AddLightQuadEntrypoint : public AddLightEntrypoint<QuadLight>
{
public:
    explicit AddLightQuadEntrypoint(ModelManager &models);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class AddLightSphereEntrypoint : public AddLightEntrypoint<SphereLight>
{
public:
    explicit AddLightSphereEntrypoint(ModelManager &models);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};
} // namespace brayns
