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
#include <brayns/engine/json/adapters/GeometryAdapters.h>
#include <brayns/engine/scene/Scene.h>
#include <brayns/network/adapters/ModelInstanceAdapter.h>
#include <brayns/network/entrypoint/Entrypoint.h>

namespace brayns
{
template<typename T>
class AddGeometryEntrypoint : public Entrypoint<std::vector<GeometryWithColor<T>>, ModelInstance>
{
public:
    using Request = typename Entrypoint<std::vector<GeometryWithColor<T>>, ModelInstance>::Request;

    AddGeometryEntrypoint(Scene &scene)
        : _scene(scene)
    {
    }

    void onRequest(const Request &request) override
    {
        auto params = request.getParams();
        auto numGeometries = params.size();

        std::vector<T> geometries;
        geometries.reserve(numGeometries);
        std::vector<brayns::Vector4f> colors;
        colors.reserve(numGeometries);

        for (size_t i = 0; i < numGeometries; ++i)
        {
            auto &param = params[i];
            geometries.push_back(std::move(param.geometry));
            colors.push_back(param.color);
        }

        auto newModel = std::make_unique<Model>();
        auto &renderComponent = newModel->addComponent<GeometryRendererComponent>(std::move(geometries));
        renderComponent.setColors(colors);

        auto &instance = _scene.addModel({}, std::move(newModel));
        request.reply(instance);
    }

private:
    Scene &_scene;
};

class AddBoxesEntrypoint final : public AddGeometryEntrypoint<Box>
{
public:
    AddBoxesEntrypoint(Scene &scene);

    std::string getMethod() const override;
    std::string getDescription() const override;
};

class AddPlanesEntrypoint final : public AddGeometryEntrypoint<Plane>
{
public:
    AddPlanesEntrypoint(Scene &scene);

    std::string getMethod() const override;
    std::string getDescription() const override;
};

class AddCapsulesEntrypoint final : public AddGeometryEntrypoint<Capsule>
{
public:
    AddCapsulesEntrypoint(Scene &scene);

    std::string getMethod() const override;
    std::string getDescription() const override;
};

class AddSpheresEntrypoint final : public AddGeometryEntrypoint<Sphere>
{
public:
    AddSpheresEntrypoint(Scene &scene);

    std::string getMethod() const override;
    std::string getDescription() const override;
};
}
