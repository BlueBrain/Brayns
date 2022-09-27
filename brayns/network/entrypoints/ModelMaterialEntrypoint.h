/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include <brayns/engine/material/Material.h>
#include <brayns/engine/scene/ModelManager.h>
#include <brayns/network/common/ExtractModel.h>
#include <brayns/network/entrypoint/Entrypoint.h>
#include <brayns/network/messages/GetModelMessage.h>
#include <brayns/network/messages/MaterialMessage.h>

namespace brayns
{
class GetMaterialType : public Entrypoint<GetModelMessage, std::string>
{
public:
    GetMaterialType(ModelManager &models);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
    virtual void onRequest(const Request &request) override;

private:
    ModelManager &_models;
};

template<typename MaterialType>
class SetMaterialEntrypoint : public Entrypoint<SetMaterialMessage<MaterialType>, EmptyMessage>
{
public:
    using Request = typename Entrypoint<SetMaterialMessage<MaterialType>, EmptyMessage>::Request;

    SetMaterialEntrypoint(ModelManager &models)
        : _models(models)
    {
    }

    void onRequest(const Request &request) override
    {
        auto params = request.getParams();
        auto modelId = params.model_id;
        auto &buffer = params.material;
        auto &instance = ExtractModel::fromId(_models, modelId);
        Model &model = instance.getModel();
        auto &components = model.getComponents();

        auto material = components.find<Material>();
        if (!material)
        {
            throw JsonRpcException("The model does not have material");
        }

        MaterialType materialData;
        buffer.extract(materialData);
        *material = Material(std::move(materialData));

        request.reply(EmptyMessage());
    }

private:
    ModelManager &_models;
};

class SetMaterialCarPaint : public SetMaterialEntrypoint<CarPaint>
{
public:
    SetMaterialCarPaint(ModelManager &ModelManager);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class SetMaterialPhong : public SetMaterialEntrypoint<Phong>
{
public:
    SetMaterialPhong(ModelManager &ModelManager);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class SetMaterialEmissive : public SetMaterialEntrypoint<Emissive>
{
public:
    SetMaterialEmissive(ModelManager &ModelManager);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class SetMaterialGlass : public SetMaterialEntrypoint<Glass>
{
public:
    SetMaterialGlass(ModelManager &ModelManager);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class SetMaterialMatte : public SetMaterialEntrypoint<Matte>
{
public:
    SetMaterialMatte(ModelManager &ModelManager);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class SetMaterialMetal : public SetMaterialEntrypoint<Metal>
{
public:
    SetMaterialMetal(ModelManager &ModelManager);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class SetMaterialPlastic : public SetMaterialEntrypoint<Plastic>
{
public:
    SetMaterialPlastic(ModelManager &ModelManager);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

template<typename MaterialType>
class GetMaterialEntrypoint : public Entrypoint<GetModelMessage, MaterialType>
{
public:
    using Request = typename Entrypoint<GetModelMessage, MaterialType>::Request;

    GetMaterialEntrypoint(ModelManager &models)
        : _models(models)
    {
    }

    virtual void onRequest(const Request &request) override
    {
        auto params = request.getParams();
        auto modelId = params.id;
        auto &instance = ExtractModel::fromId(_models, modelId);
        Model &model = instance.getModel();
        auto &components = model.getComponents();

        auto material = components.find<Material>();
        if (!material)
        {
            throw JsonRpcException("The model does not have material");
        }

        if (auto cast = material->as<MaterialType>())
        {
            request.reply(*cast);
            return;
        }
        throw InvalidRequestException("Invalid material type (should be '" + material->getName() + "')");
    }

private:
    ModelManager &_models;
};

class GetMaterialCarPaint : public GetMaterialEntrypoint<CarPaint>
{
public:
    GetMaterialCarPaint(ModelManager &scene);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class GetMaterialPhong : public GetMaterialEntrypoint<Phong>
{
public:
    GetMaterialPhong(ModelManager &scene);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class GetMaterialEmissive : public GetMaterialEntrypoint<Emissive>
{
public:
    GetMaterialEmissive(ModelManager &scene);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class GetMaterialGlass : public GetMaterialEntrypoint<Glass>
{
public:
    GetMaterialGlass(ModelManager &scene);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class GetMaterialMatte : public GetMaterialEntrypoint<Matte>
{
public:
    GetMaterialMatte(ModelManager &scene);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class GetMaterialMetal : public GetMaterialEntrypoint<Metal>
{
public:
    GetMaterialMetal(ModelManager &scene);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class GetMaterialPlastic : public GetMaterialEntrypoint<Plastic>
{
public:
    GetMaterialPlastic(ModelManager &scene);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};
}
