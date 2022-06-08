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

#include "ModelMaterialEntrypoint.h"

namespace brayns
{
GetMaterialType::GetMaterialType(Scene &scene)
    : _modelManager(scene)
{
}

std::string GetMaterialType::getMethod() const
{
    return "get-material-type";
}

std::string GetMaterialType::getDescription() const
{
    return "Returns the type of the material of a given model, if any";
}

void GetMaterialType::onRequest(const Request &request)
{
    auto params = request.getParams();
    auto modelId = params.id;
    auto &modelInstance = _modelManager.getModelInstance(modelId);
    auto &model = modelInstance.getModel();
    auto &materialComponent = model.getComponent<MaterialComponent>();
    auto &material = materialComponent.getMaterial();
    auto name = material.getName();
    request.reply(name);
}

SetMaterialCarPaint::SetMaterialCarPaint(Scene &scene)
    : SetMaterialEntrypoint<CarPaintMaterial>(scene)
{
}

std::string SetMaterialCarPaint::getMethod() const
{
    return "set-material-carpaint";
}

std::string SetMaterialCarPaint::getDescription() const
{
    return "Updates the material of the given model to a Car paint material. "
           "This material is only usable with the production renderer";
}

SetMaterialDefault::SetMaterialDefault(Scene &scene)
    : SetMaterialEntrypoint<DefaultMaterial>(scene)
{
}

std::string SetMaterialDefault::getMethod() const
{
    return "set-material-default";
}

std::string SetMaterialDefault::getDescription() const
{
    return "Updates the material of the given model to the Default material. This material works with all renderers. "
           "It has a matte appearance.";
}

SetMaterialEmissive::SetMaterialEmissive(Scene &scene)
    : SetMaterialEntrypoint<EmissiveMaterial>(scene)
{
}

std::string SetMaterialEmissive::getMethod() const
{
    return "set-material-emissive";
}

std::string SetMaterialEmissive::getDescription() const
{
    return "Updates the material of the given model to an Emisive material. "
           "This material is only usable with the production renderer";
}

SetMaterialGlass::SetMaterialGlass(Scene &scene)
    : SetMaterialEntrypoint<GlassMaterial>(scene)
{
}

std::string SetMaterialGlass::getMethod() const
{
    return "set-material-glass";
}

std::string SetMaterialGlass::getDescription() const
{
    return "Updates the material of the given model to a Glass material. "
           "This material is only usable with the production renderer";
}

SetMaterialMatte::SetMaterialMatte(Scene &scene)
    : SetMaterialEntrypoint<MatteMaterial>(scene)
{
}

std::string SetMaterialMatte::getMethod() const
{
    return "set-material-matte";
}

std::string SetMaterialMatte::getDescription() const
{
    return "Updates the material of the given model to a Matte material. "
           "This material is only usable with the production renderer";
}

SetMaterialMetal::SetMaterialMetal(Scene &scene)
    : SetMaterialEntrypoint<MetalMaterial>(scene)
{
}

std::string SetMaterialMetal::getMethod() const
{
    return "set-material-metal";
}

std::string SetMaterialMetal::getDescription() const
{
    return "Updates the material of the given model to a Metal material. "
           "This material is only usable with the production renderer";
}

SetMaterialPlastic::SetMaterialPlastic(Scene &scene)
    : SetMaterialEntrypoint<PlasticMaterial>(scene)
{
}

std::string SetMaterialPlastic::getMethod() const
{
    return "set-material-plastic";
}

std::string SetMaterialPlastic::getDescription() const
{
    return "Updates the material of the given model to a Plastic material. "
           "This material is only usable with the production renderer";
}

GetMaterialCarPaint::GetMaterialCarPaint(Scene &scene)
    : GetMaterialEntrypoint<CarPaintMaterial>(scene)
{
}

std::string GetMaterialCarPaint::getMethod() const
{
    return "get-material-carpaint";
}

std::string GetMaterialCarPaint::getDescription() const
{
    return "Returns the material of the given model as a car paint material, if possible";
}

GetMaterialDefault::GetMaterialDefault(Scene &scene)
    : GetMaterialEntrypoint<DefaultMaterial>(scene)
{
}

std::string GetMaterialDefault::getMethod() const
{
    return "get-material-default";
}

std::string GetMaterialDefault::getDescription() const
{
    return "Returns the material of the given model as a default material, if possible";
}

GetMaterialEmissive::GetMaterialEmissive(Scene &scene)
    : GetMaterialEntrypoint<EmissiveMaterial>(scene)
{
}

std::string GetMaterialEmissive::getMethod() const
{
    return "get-material-emissive";
}

std::string GetMaterialEmissive::getDescription() const
{
    return "Returns the material of the given model as a emissive material, if possible";
}

GetMaterialGlass::GetMaterialGlass(Scene &scene)
    : GetMaterialEntrypoint<GlassMaterial>(scene)
{
}

std::string GetMaterialGlass::getMethod() const
{
    return "get-material-glass";
}

std::string GetMaterialGlass::getDescription() const
{
    return "Returns the material of the given model as a glass material, if possible";
}

GetMaterialMatte::GetMaterialMatte(Scene &scene)
    : GetMaterialEntrypoint<MatteMaterial>(scene)
{
}

std::string GetMaterialMatte::getMethod() const
{
    return "get-material-matte";
}

std::string GetMaterialMatte::getDescription() const
{
    return "Returns the material of the given model as a matte material, if possible";
}

GetMaterialMetal::GetMaterialMetal(Scene &scene)
    : GetMaterialEntrypoint<MetalMaterial>(scene)
{
}

std::string GetMaterialMetal::getMethod() const
{
    return "get-material-metal";
}

std::string GetMaterialMetal::getDescription() const
{
    return "Returns the material of the given model as a metal material, if possible";
}

GetMaterialPlastic::GetMaterialPlastic(Scene &scene)
    : GetMaterialEntrypoint<PlasticMaterial>(scene)
{
}

std::string GetMaterialPlastic::getMethod() const
{
    return "get-material-plastic";
}

std::string GetMaterialPlastic::getDescription() const
{
    return "Returns the material of the given model as a plastic material, if possible";
}
}
