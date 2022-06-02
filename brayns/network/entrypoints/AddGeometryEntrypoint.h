/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/engine/components/GeometryRendererComponent.h>
#include <brayns/engine/scenecomponents/SceneModelManager.h>
#include <brayns/network/adapters/GeometryAdapter.h>
#include <brayns/network/adapters/ModelInstanceAdapter.h>
#include <brayns/network/entrypoint/Entrypoint.h>

namespace brayns
{
template<typename T>
class AddGeometryEntrypoint : public Entrypoint<std::vector<GeometryWithColor<T>>, ModelInstance>
{
public:
    using Request = typename Entrypoint<std::vector<GeometryWithColor<T>>, ModelInstance>::Request;

    AddGeometryEntrypoint(SceneModelManager &models)
        : _models(models)
    {
    }

    void onRequest(const Request &request) override
    {
        auto params = request.getParams();
        auto numGeometries = params.size();

        std::vector<T> geometries(numGeometries);
        std::vector<brayns::Vector4f> colors(numGeometries);

        for (size_t i = 0; i < numGeometries; ++i)
        {
            auto &param = params[i];
            geometries[i] = std::move(param.geometry);
            colors[i] = std::move(param.color);
        }

        auto newModel = std::make_unique<Model>();
        auto &renderComponent = newModel->addComponent<GeometryRendererComponent<T>>(std::move(geometries));
        renderComponent.setColors(colors);

        auto &instance = _models.addModel({}, std::move(newModel));

        request.reply(instance);
    }

private:
    SceneModelManager &_models;
};

class AddBoxesEntrypoint final : public AddGeometryEntrypoint<Box>
{
public:
    AddBoxesEntrypoint(SceneModelManager &models);

    std::string getMethod() const override;
    std::string getDescription() const override;
};

class AddPlanesEntrypoint final : public AddGeometryEntrypoint<Plane>
{
public:
    AddPlanesEntrypoint(SceneModelManager &models);

    std::string getMethod() const override;
    std::string getDescription() const override;
};

class AddCapsulesEntrypoint final : public AddGeometryEntrypoint<Primitive>
{
public:
    AddCapsulesEntrypoint(SceneModelManager &models);

    std::string getMethod() const override;
    std::string getDescription() const override;
};

class AddSpheresEntrypoint final : public AddGeometryEntrypoint<Sphere>
{
public:
    AddSpheresEntrypoint(SceneModelManager &models);

    std::string getMethod() const override;
    std::string getDescription() const override;
};
}
