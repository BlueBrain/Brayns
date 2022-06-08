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

#include <brayns/engine/Scene.h>
#include <brayns/engine/components/MaterialComponent.h>

#include <brayns/network/messages/GetModelMessage.h>
#include <brayns/network/messages/MaterialMessage.h>

#include <brayns/network/entrypoint/Entrypoint.h>

namespace brayns
{
class GetMaterialType : public Entrypoint<GetModelMessage, std::string>
{
public:
    GetMaterialType(Scene &scene);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
    virtual void onRequest(const Request &request) override;

private:
    Scene &_modelManager;
};

template<typename MaterialType>
class SetMaterialEntrypoint : public Entrypoint<SetMaterialMessage<MaterialType>, EmptyMessage>
{
public:
    using Request = typename Entrypoint<SetMaterialMessage<MaterialType>, EmptyMessage>::Request;

    SetMaterialEntrypoint(Scene &Scene)
        : _scene(Scene)
    {
    }

    void onRequest(const Request &request) override
    {
        auto params = request.getParams();
        auto modelId = params.model_id;
        auto &buffer = params.material;
        auto &instance = _scene.getModelInstance(modelId);
        Model &model = instance.getModel();
        auto &component = model.getComponent<MaterialComponent>();
        auto material = std::make_unique<MaterialType>();
        buffer.extract(*material);
        component.setMaterial(std::move(material));
        request.reply(EmptyMessage());
    }

private:
    Scene &_scene;
};

class SetMaterialCarPaint : public SetMaterialEntrypoint<CarPaintMaterial>
{
public:
    SetMaterialCarPaint(Scene &Scene);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class SetMaterialDefault : public SetMaterialEntrypoint<DefaultMaterial>
{
public:
    SetMaterialDefault(Scene &Scene);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class SetMaterialEmissive : public SetMaterialEntrypoint<EmissiveMaterial>
{
public:
    SetMaterialEmissive(Scene &Scene);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class SetMaterialGlass : public SetMaterialEntrypoint<GlassMaterial>
{
public:
    SetMaterialGlass(Scene &Scene);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class SetMaterialMatte : public SetMaterialEntrypoint<MatteMaterial>
{
public:
    SetMaterialMatte(Scene &Scene);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class SetMaterialMetal : public SetMaterialEntrypoint<MetalMaterial>
{
public:
    SetMaterialMetal(Scene &Scene);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class SetMaterialPlastic : public SetMaterialEntrypoint<PlasticMaterial>
{
public:
    SetMaterialPlastic(Scene &Scene);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

template<typename MaterialType>
class GetMaterialEntrypoint : public Entrypoint<GetModelMessage, MaterialType>
{
public:
    using Request = typename Entrypoint<GetModelMessage, MaterialType>::Request;

    GetMaterialEntrypoint(Scene &scene)
        : _scene(scene)
    {
    }

    virtual void onRequest(const Request &request) override
    {
        auto params = request.getParams();
        auto modelId = params.id;
        auto &instance = _scene.getModelInstance(modelId);
        Model &model = instance.getModel();
        auto &component = model.getComponent<MaterialComponent>();
        auto &material = component.getMaterial();
        try
        {
            auto &castedMaterial = dynamic_cast<MaterialType &>(material);
            request.reply(castedMaterial);
        }
        catch (const std::bad_cast &)
        {
            throw InvalidRequestException("Invalid material type (should be '" + material.getName() + "')");
        }
    }

private:
    Scene &_scene;
};

class GetMaterialCarPaint : public GetMaterialEntrypoint<CarPaintMaterial>
{
public:
    GetMaterialCarPaint(Scene &scene);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class GetMaterialDefault : public GetMaterialEntrypoint<DefaultMaterial>
{
public:
    GetMaterialDefault(Scene &scene);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class GetMaterialEmissive : public GetMaterialEntrypoint<EmissiveMaterial>
{
public:
    GetMaterialEmissive(Scene &scene);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class GetMaterialGlass : public GetMaterialEntrypoint<GlassMaterial>
{
public:
    GetMaterialGlass(Scene &scene);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class GetMaterialMatte : public GetMaterialEntrypoint<MatteMaterial>
{
public:
    GetMaterialMatte(Scene &scene);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class GetMaterialMetal : public GetMaterialEntrypoint<MetalMaterial>
{
public:
    GetMaterialMetal(Scene &scene);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class GetMaterialPlastic : public GetMaterialEntrypoint<PlasticMaterial>
{
public:
    GetMaterialPlastic(Scene &scene);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};
}
