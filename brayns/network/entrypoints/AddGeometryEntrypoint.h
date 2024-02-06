/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <brayns/engine/colormethods/PrimitiveColorMethod.h>
#include <brayns/engine/colormethods/SolidColorMethod.h>
#include <brayns/engine/components/ColorList.h>
#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/components/GeometryViews.h>
#include <brayns/engine/json/adapters/GeometryAdapters.h>
#include <brayns/engine/json/adapters/ModelInstanceAdapter.h>
#include <brayns/engine/scene/ModelManager.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>
#include <brayns/engine/systems/GenericColorSystem.h>
#include <brayns/engine/systems/GeometryDataSystem.h>

#include <brayns/network/entrypoint/Entrypoint.h>

namespace brayns
{
template<typename T>
class AddGeometryEntrypoint : public Entrypoint<std::vector<GeometryWithColor<T>>, ModelInstance>
{
public:
    using Request = typename Entrypoint<std::vector<GeometryWithColor<T>>, ModelInstance>::Request;

    explicit AddGeometryEntrypoint(ModelManager &models):
        _models(models)
    {
    }

    void onRequest(const Request &request) override
    {
        auto model = std::make_shared<Model>("geometry");

        auto [primitives, colors] = _unpackRequest(request);
        auto primitiveCount = primitives.size();

        auto &geometry = _addGeometry(*model, std::move(primitives));
        auto &view = _addView(*model, geometry);
        auto &colorList = _addColors(*model, std::move(colors));

        view.setColorPerPrimitive(ospray::cpp::SharedData(colorList));

        _setUpSystems(*model, primitiveCount);

        auto *instance = _models.add(std::move(model));
        request.reply(*instance);
    }

private:
    std::tuple<std::vector<T>, std::vector<brayns::Vector4f>> _unpackRequest(const Request &request)
    {
        auto params = request.getParams();
        auto numPrimitives = params.size();

        std::vector<T> primitives;
        primitives.reserve(numPrimitives);
        std::vector<brayns::Vector4f> colors;
        colors.reserve(numPrimitives);

        for (size_t i = 0; i < numPrimitives; ++i)
        {
            auto &param = params[i];
            primitives.push_back(std::move(param.geometry));
            colors.push_back(param.color);
        }

        return std::make_tuple(std::move(primitives), std::move(colors));
    }

    Geometry &_addGeometry(Model &model, std::vector<T> primitives)
    {
        auto &components = model.getComponents();
        auto &geometries = components.add<Geometries>();
        return geometries.elements.emplace_back(std::move(primitives));
    }

    GeometryView &_addView(Model &model, const Geometry &geometry)
    {
        auto &components = model.getComponents();
        auto &views = components.add<GeometryViews>();
        return views.elements.emplace_back(geometry);
    }

    const std::vector<brayns::Vector4f> &_addColors(Model &model, std::vector<brayns::Vector4f> inputColors)
    {
        auto &components = model.getComponents();
        auto &colorList = components.add<ColorList>(std::move(inputColors));
        return colorList.elements;
    }

    void _setUpSystems(Model &model, size_t numGeometries)
    {
        auto colorMethods = std::vector<std::unique_ptr<IColorMethod>>();
        colorMethods.push_back(std::make_unique<SolidColorMethod>());
        colorMethods.push_back(std::make_unique<PrimitiveColorMethod>("primitive", numGeometries));

        auto &systems = model.getSystems();
        systems.setBoundsSystem<GenericBoundsSystem<Geometries>>();
        systems.setDataSystem<GeometryDataSystem>();
        systems.setColorSystem<GenericColorSystem>(std::move(colorMethods));
    }

private:
    ModelManager &_models;
};

class AddBoundedPlanesEntrypoint final : public AddGeometryEntrypoint<BoundedPlane>
{
public:
    explicit AddBoundedPlanesEntrypoint(ModelManager &models);

    std::string getMethod() const override;
    std::string getDescription() const override;
};

class AddBoxesEntrypoint final : public AddGeometryEntrypoint<Box>
{
public:
    explicit AddBoxesEntrypoint(ModelManager &models);

    std::string getMethod() const override;
    std::string getDescription() const override;
};

class AddPlanesEntrypoint final : public AddGeometryEntrypoint<Plane>
{
public:
    explicit AddPlanesEntrypoint(ModelManager &models);

    std::string getMethod() const override;
    std::string getDescription() const override;
};

class AddCapsulesEntrypoint final : public AddGeometryEntrypoint<Capsule>
{
public:
    explicit AddCapsulesEntrypoint(ModelManager &models);

    std::string getMethod() const override;
    std::string getDescription() const override;
};

class AddSpheresEntrypoint final : public AddGeometryEntrypoint<Sphere>
{
public:
    explicit AddSpheresEntrypoint(ModelManager &models);

    std::string getMethod() const override;
    std::string getDescription() const override;
};
}
