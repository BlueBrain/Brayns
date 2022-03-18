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

#include <brayns/engine/defaultcomponents/MaterialComponent.h>
#include <brayns/engine/scenecomponents/SceneModelManager.h>

#include <brayns/network/adapters/MaterialAdapter.h>
#include <brayns/network/adapters/ModelInstanceAdapter.h>

#include <brayns/network/entrypoint/Entrypoint.h>

namespace brayns
{
class GetMaterialType : public Entrypoint<GetModelMessage, std::string>
{
public:
    GetMaterialType(SceneModelManager &modelManager);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
    virtual void onRequest(const Request &request) override;
private:
    SceneModelManager &_modelManager;
};

template<typename MaterialType>
class SetMaterialEntrypoint : public Entrypoint<ModelMaterial<MaterialType>, EmptyMessage>
{
public:
    SetMaterialEntrypoint(SceneModelManager& sceneModelManager)
     : _sceneModelManager(sceneModelManager)
    {
    }

    void onRequest(const typename Entrypoint<ModelMaterial<MaterialType>, EmptyMessage>::Request &request) override
    {
        ModelMaterial<MaterialType> updater (_sceneModelManager);
        request.getParams(updater);
        updater.update();
        request.reply(EmptyMessage());
    }

private:
    SceneModelManager &_sceneModelManager;
};

class SetMaterialCarPaint : public SetMaterialEntrypoint<CarPaintMaterial>
{
public:
    SetMaterialCarPaint(SceneModelManager &sceneModelManager);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class SetMaterialDefault : public SetMaterialEntrypoint<DefaultMaterial>
{
public:
    SetMaterialDefault(SceneModelManager &sceneModelManager);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class SetMaterialEmissive : public SetMaterialEntrypoint<EmissiveMaterial>
{
public:
    SetMaterialEmissive(SceneModelManager &sceneModelManager);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class SetMaterialGlass : public SetMaterialEntrypoint<GlassMaterial>
{
public:
    SetMaterialGlass(SceneModelManager &sceneModelManager);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class SetMaterialMatte : public SetMaterialEntrypoint<MatteMaterial>
{
public:
    SetMaterialMatte(SceneModelManager &sceneModelManager);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class SetMaterialMetal : public SetMaterialEntrypoint<MetalMaterial>
{
public:
    SetMaterialMetal(SceneModelManager &sceneModelManager);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class SetMaterialPlastic : public SetMaterialEntrypoint<PlasticMaterial>
{
public:
    SetMaterialPlastic(SceneModelManager &sceneModelManager);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

template<typename MaterialType>
class GetMaterialEntrypoint : public Entrypoint<GetModelMessage, MaterialType>
{
public:
    GetMaterialEntrypoint(SceneModelManager &modelManager)
     : _modelManager(modelManager)
    {
    }

    virtual void onRequest(const typename Entrypoint<GetModelMessage, MaterialType>::Request &request) override
    {
        auto params = request.getParams();
        auto modelId = params.id;
        auto &modelInstance = _modelManager.getModelInstance(modelId);
        Model &model = modelInstance.getModel();
        auto &materialComponent = model.getComponent<MaterialComponent>();
        auto &material = materialComponent.getMaterial();
        try
        {
            auto &castedMaterial = dynamic_cast<MaterialType&>(material);
            request.reply(castedMaterial);
        }
        catch(const std::bad_cast &)
        {
            throw JsonRpcException("The material cannot be casted to the requested type");
        }
    }

private:
    SceneModelManager &_modelManager;
};

class GetMaterialCarPaint : public GetMaterialEntrypoint<CarPaintMaterial>
{
public:
    GetMaterialCarPaint(SceneModelManager &modelManager);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class GetMaterialDefault : public GetMaterialEntrypoint<DefaultMaterial>
{
public:
    GetMaterialDefault(SceneModelManager &modelManager);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class GetMaterialEmissive : public GetMaterialEntrypoint<EmissiveMaterial>
{
public:
    GetMaterialEmissive(SceneModelManager &modelManager);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class GetMaterialGlass : public GetMaterialEntrypoint<GlassMaterial>
{
public:
    GetMaterialGlass(SceneModelManager &modelManager);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class GetMaterialMatte : public GetMaterialEntrypoint<MatteMaterial>
{
public:
    GetMaterialMatte(SceneModelManager &modelManager);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class GetMaterialMetal : public GetMaterialEntrypoint<MetalMaterial>
{
public:
    GetMaterialMetal(SceneModelManager &modelManager);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};

class GetMaterialPlastic : public GetMaterialEntrypoint<PlasticMaterial>
{
public:
    GetMaterialPlastic(SceneModelManager &modelManager);
    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
};
}
