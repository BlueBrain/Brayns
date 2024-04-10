/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include <brayns/core/engine/material/Material.h>
#include <brayns/core/engine/scene/ModelManager.h>
#include <brayns/core/network/common/ExtractModel.h>
#include <brayns/core/network/entrypoint/Entrypoint.h>
#include <brayns/core/network/messages/GetModelMessage.h>
#include <brayns/core/network/messages/MaterialMessage.h>

namespace brayns
{
class GetMaterialType : public Entrypoint<GetModelMessage, std::string>
{
public:
    explicit GetMaterialType(ModelManager &models);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
    virtual void onRequest(const Request &request) override;

private:
    ModelManager &_models;
};

template<typename MaterialType>
class SetMaterialEntrypoint : public Entrypoint<SetMaterialMessage<MaterialType>, EmptyJson>
{
public:
    using Request = typename Entrypoint<SetMaterialMessage<MaterialType>, EmptyJson>::Request;

    explicit SetMaterialEntrypoint(ModelManager &models):
        _models(models)
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

        request.reply(EmptyJson());
    }

private:
    ModelManager &_models;
};

class SetMaterialCarPaint : public SetMaterialEntrypoint<CarPaint>
{
public:
    explicit SetMaterialCarPaint(ModelManager &ModelManager);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class SetMaterialPhong : public SetMaterialEntrypoint<Phong>
{
public:
    explicit SetMaterialPhong(ModelManager &ModelManager);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class SetMaterialEmissive : public SetMaterialEntrypoint<Emissive>
{
public:
    explicit SetMaterialEmissive(ModelManager &ModelManager);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class SetMaterialGlass : public SetMaterialEntrypoint<Glass>
{
public:
    explicit SetMaterialGlass(ModelManager &ModelManager);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class SetMaterialMatte : public SetMaterialEntrypoint<Matte>
{
public:
    explicit SetMaterialMatte(ModelManager &ModelManager);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class SetMaterialMetal : public SetMaterialEntrypoint<Metal>
{
public:
    explicit SetMaterialMetal(ModelManager &ModelManager);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class SetMaterialPlastic : public SetMaterialEntrypoint<Plastic>
{
public:
    explicit SetMaterialPlastic(ModelManager &ModelManager);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class SetMaterialPrincipled : public SetMaterialEntrypoint<Principled>
{
public:
    explicit SetMaterialPrincipled(ModelManager &ModelManager);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

template<typename MaterialType>
class GetMaterialEntrypoint : public Entrypoint<GetModelMessage, MaterialType>
{
public:
    using Request = typename Entrypoint<GetModelMessage, MaterialType>::Request;

    explicit GetMaterialEntrypoint(ModelManager &models):
        _models(models)
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

        auto cast = material->as<MaterialType>();
        if (!cast)
        {
            throw InvalidRequestException("Invalid material type (should be '" + material->getName() + "')");
        }
        request.reply(*cast);
    }

private:
    ModelManager &_models;
};

class GetMaterialCarPaint : public GetMaterialEntrypoint<CarPaint>
{
public:
    explicit GetMaterialCarPaint(ModelManager &scene);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class GetMaterialPhong : public GetMaterialEntrypoint<Phong>
{
public:
    explicit GetMaterialPhong(ModelManager &scene);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class GetMaterialEmissive : public GetMaterialEntrypoint<Emissive>
{
public:
    explicit GetMaterialEmissive(ModelManager &scene);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class GetMaterialGlass : public GetMaterialEntrypoint<Glass>
{
public:
    explicit GetMaterialGlass(ModelManager &scene);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class GetMaterialMatte : public GetMaterialEntrypoint<Matte>
{
public:
    explicit GetMaterialMatte(ModelManager &scene);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class GetMaterialMetal : public GetMaterialEntrypoint<Metal>
{
public:
    explicit GetMaterialMetal(ModelManager &scene);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class GetMaterialPlastic : public GetMaterialEntrypoint<Plastic>
{
public:
    explicit GetMaterialPlastic(ModelManager &scene);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class GetMaterialPrincipled : public GetMaterialEntrypoint<Principled>
{
public:
    explicit GetMaterialPrincipled(ModelManager &scene);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};
}
