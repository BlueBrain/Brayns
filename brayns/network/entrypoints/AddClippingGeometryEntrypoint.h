/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/json/adapters/GeometryAdapters.h>
#include <brayns/engine/json/adapters/ModelInstanceAdapter.h>
#include <brayns/engine/scene/ModelManager.h>
#include <brayns/engine/systems/ClipperInitSystem.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>

#include <brayns/network/entrypoint/Entrypoint.h>

namespace brayns
{
template<typename T>
class AddClippingGeometryEntrypoint : public Entrypoint<std::vector<T>, ModelInstance>
{
public:
    using Request = typename Entrypoint<std::vector<T>, ModelInstance>::Request;

    explicit AddClippingGeometryEntrypoint(ModelManager &models)
        : _models(models)
    {
    }

    void onRequest(const Request &request) override
    {
        auto model = std::make_shared<Model>("clipping_geometry");

        _addGeometry(*model, request.getParams());
        _setUpSystems(*model);

        auto instance = _models.add(std::move(model));
        request.reply(*instance);
    }

private:
    void _addGeometry(Model &model, std::vector<T> primitives)
    {
        auto &components = model.getComponents();
        components.add<Geometries>(std::move(primitives));
    }

    void _setUpSystems(Model &model)
    {
        auto &systems = model.getSystems();
        systems.setBoundsSystem<GenericBoundsSystem<Geometries>>();
        systems.setInitSystem<ClipperInitSystem>();
    }

private:
    ModelManager &_models;
};

class AddClippingBoundedPlanesEntrypoint final : public AddClippingGeometryEntrypoint<BoundedPlane>
{
public:
    explicit AddClippingBoundedPlanesEntrypoint(ModelManager &models);

    std::string getMethod() const override;
    std::string getDescription() const override;
};

class AddClippingBoxesEntrypoint final : public AddClippingGeometryEntrypoint<Box>
{
public:
    explicit AddClippingBoxesEntrypoint(ModelManager &models);

    std::string getMethod() const override;
    std::string getDescription() const override;
};

class AddClippingPlanesEntrypoint final : public AddClippingGeometryEntrypoint<Plane>
{
public:
    explicit AddClippingPlanesEntrypoint(ModelManager &models);

    std::string getMethod() const override;
    std::string getDescription() const override;
};

class AddClippingCapsulesEntrypoint final : public AddClippingGeometryEntrypoint<Capsule>
{
public:
    explicit AddClippingCapsulesEntrypoint(ModelManager &models);

    std::string getMethod() const override;
    std::string getDescription() const override;
};

class AddClippingSpheresEntrypoint final : public AddClippingGeometryEntrypoint<Sphere>
{
public:
    explicit AddClippingSpheresEntrypoint(ModelManager &models);

    std::string getMethod() const override;
    std::string getDescription() const override;
};
}
