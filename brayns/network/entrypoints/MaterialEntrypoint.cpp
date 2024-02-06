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

#include "MaterialEntrypoint.h"

namespace brayns
{
GetMaterialType::GetMaterialType(ModelManager &models):
    _models(models)
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
    auto &modelInstance = ExtractModel::fromId(_models, modelId);
    auto &model = modelInstance.getModel();
    auto &components = model.getComponents();
    if (auto material = components.find<Material>())
    {
        request.reply(material->getName());
        return;
    }
    throw JsonRpcException("The model does not have material");
}

SetMaterialCarPaint::SetMaterialCarPaint(ModelManager &models):
    SetMaterialEntrypoint<CarPaint>(models)
{
}

std::string SetMaterialCarPaint::getMethod() const
{
    return "set-material-carpaint";
}

std::string SetMaterialCarPaint::getDescription() const
{
    return "Updates the material of the given model to a car paint material. "
           "This material is only usable with the production renderer";
}

SetMaterialPhong::SetMaterialPhong(ModelManager &models):
    SetMaterialEntrypoint<Phong>(models)
{
}

std::string SetMaterialPhong::getMethod() const
{
    return "set-material-phong";
}

std::string SetMaterialPhong::getDescription() const
{
    return "Updates the material of the given model to the phong material. This material works with all renderers. "
           "It has a matte appearance.";
}

SetMaterialEmissive::SetMaterialEmissive(ModelManager &models):
    SetMaterialEntrypoint<Emissive>(models)
{
}

std::string SetMaterialEmissive::getMethod() const
{
    return "set-material-emissive";
}

std::string SetMaterialEmissive::getDescription() const
{
    return "Updates the material of the given model to an emisive material. "
           "This material is only usable with the production renderer";
}

SetMaterialGlass::SetMaterialGlass(ModelManager &models):
    SetMaterialEntrypoint<Glass>(models)
{
}

std::string SetMaterialGlass::getMethod() const
{
    return "set-material-glass";
}

std::string SetMaterialGlass::getDescription() const
{
    return "Updates the material of the given model to a glass material. "
           "This material is only usable with the production renderer";
}

SetMaterialGhost::SetMaterialGhost(ModelManager &models):
    SetMaterialEntrypoint<Ghost>(models)
{
}

std::string SetMaterialGhost::getMethod() const
{
    return "set-material-ghost";
}

std::string SetMaterialGhost::getDescription() const
{
    return "Updates the material of the given model to a ghost material. "
           "The ghost effect is only visible with the interactive renderer.";
}

SetMaterialMatte::SetMaterialMatte(ModelManager &models):
    SetMaterialEntrypoint<Matte>(models)
{
}

std::string SetMaterialMatte::getMethod() const
{
    return "set-material-matte";
}

std::string SetMaterialMatte::getDescription() const
{
    return "Updates the material of the given model to a matte material. "
           "This material is only usable with the production renderer";
}

SetMaterialMetal::SetMaterialMetal(ModelManager &models):
    SetMaterialEntrypoint<Metal>(models)
{
}

std::string SetMaterialMetal::getMethod() const
{
    return "set-material-metal";
}

std::string SetMaterialMetal::getDescription() const
{
    return "Updates the material of the given model to a metal material. "
           "This material is only usable with the production renderer";
}

SetMaterialPlastic::SetMaterialPlastic(ModelManager &models):
    SetMaterialEntrypoint<Plastic>(models)
{
}

std::string SetMaterialPlastic::getMethod() const
{
    return "set-material-plastic";
}

std::string SetMaterialPlastic::getDescription() const
{
    return "Updates the material of the given model to a plastic material. "
           "This material is only usable with the production renderer";
}

SetMaterialPrincipled::SetMaterialPrincipled(ModelManager &models):
    SetMaterialEntrypoint<Principled>(models)
{
}

std::string SetMaterialPrincipled::getMethod() const
{
    return "set-material-principled";
}

std::string SetMaterialPrincipled::getDescription() const
{
    return "Updates the material of the given model to a principled material. "
           "This material is only usable with the production renderer";
}

GetMaterialCarPaint::GetMaterialCarPaint(ModelManager &models):
    GetMaterialEntrypoint<CarPaint>(models)
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

GetMaterialPhong::GetMaterialPhong(ModelManager &models):
    GetMaterialEntrypoint<Phong>(models)
{
}

std::string GetMaterialPhong::getMethod() const
{
    return "get-material-phong";
}

std::string GetMaterialPhong::getDescription() const
{
    return "Returns the material of the given model as a phong material, if possible";
}

GetMaterialEmissive::GetMaterialEmissive(ModelManager &models):
    GetMaterialEntrypoint<Emissive>(models)
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

GetMaterialGlass::GetMaterialGlass(ModelManager &models):
    GetMaterialEntrypoint<Glass>(models)
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

GetMaterialGhost::GetMaterialGhost(ModelManager &scene):
    GetMaterialEntrypoint<Ghost>(scene)
{
}

std::string GetMaterialGhost::getMethod() const
{
    return "get-material-ghost";
}

std::string GetMaterialGhost::getDescription() const
{
    return "Returns the material of the given model as a ghost material, if possible";
}

GetMaterialMatte::GetMaterialMatte(ModelManager &models):
    GetMaterialEntrypoint<Matte>(models)
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

GetMaterialMetal::GetMaterialMetal(ModelManager &models):
    GetMaterialEntrypoint<Metal>(models)
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

GetMaterialPlastic::GetMaterialPlastic(ModelManager &models):
    GetMaterialEntrypoint<Plastic>(models)
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

GetMaterialPrincipled::GetMaterialPrincipled(ModelManager &models):
    GetMaterialEntrypoint<Principled>(models)
{
}

std::string GetMaterialPrincipled::getMethod() const
{
    return "get-material-principled";
}

std::string GetMaterialPrincipled::getDescription() const
{
    return "Returns the material of the given model as a principled material, if possible";
}
}
