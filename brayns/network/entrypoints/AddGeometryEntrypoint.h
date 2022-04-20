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

#include <brayns/engine/Scene.h>
#include <brayns/engine/components/GeometryRendererComponent.h>
#include <brayns/network/adapters/GeometryAdapter.h>
#include <brayns/network/adapters/ModelInstanceAdapter.h>
#include <brayns/network/entrypoint/Entrypoint.h>

namespace brayns
{
template<typename T>
class AddGeometryEntrypoint : public Entrypoint<std::vector<AddGeometryProxy<T>>, ModelInstanceProxy>
{
public:
    AddGeometryEntrypoint(Scene &scene)
        : _scene(scene)
    {
    }

    void onRequest(
        const typename Entrypoint<std::vector<AddGeometryProxy<T>>, ModelInstanceProxy>::Request &req) override
    {
        auto params = req.getParams();
        auto numGeometries = params.size();

        std::vector<T> geometries(numGeometries);
        std::vector<brayns::Vector4f> colors(numGeometries);

        for (size_t i = 0; i < numGeometries; ++i)
        {
            auto &proxy = params[i];
            geometries[i] = std::move(proxy.geometry);
            colors[i] = std::move(proxy.color);
        }

        auto newModel = std::make_unique<Model>();
        auto &renderComponent = newModel->addComponent<GeometryRendererComponent<T>>(std::move(geometries));
        renderComponent.setColors(colors);

        auto &modelManager = _scene.getModelManager();
        auto &instance = modelManager.addModel({}, std::move(newModel));
        // Need to compute bounds here to make sure the bounds will be updated for the next call (which may need them)
        _scene.computeBounds();

        ModelInstanceProxy result(instance);
        req.reply(result);
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

class AddCapsulesEntrypoint final : public AddGeometryEntrypoint<Primitive>
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
