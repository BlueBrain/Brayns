/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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
GetMaterialEntrypoint::GetMaterialEntrypoint(Scene &scene)
    : _scene(scene)
{
}

std::string GetMaterialEntrypoint::getName() const
{
    return "get-material";
}

std::string GetMaterialEntrypoint::getDescription() const
{
    return "Retreive the material with given ID in given model";
}

void GetMaterialEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    auto modelId = params.model_id;
    auto materialId = params.material_id;
    MaterialProxy material(_scene);
    material.setModelId(modelId);
    material.setMaterialId(materialId);
    request.reply(material);
}

SetMaterialEntrypoint::SetMaterialEntrypoint(Scene &scene)
    : _scene(scene)
{
}

std::string SetMaterialEntrypoint::getName() const
{
    return "set-material";
}

std::string SetMaterialEntrypoint::getDescription() const
{
    return "Update the corresponding material with the given properties";
}

void SetMaterialEntrypoint::onRequest(const Request &request)
{
    MaterialProxy material(_scene);
    request.getParams(material);
    material.commit();
    _scene.markModified();
    request.reply(EmptyMessage());
}
} // namespace brayns
