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

#include "AddLightEntrypoint.h"

#include <brayns/network/jsonrpc/JsonRpcException.h>

namespace brayns
{
void AddLightHelper::load(LightManager &lights, LightPtr light, const JsonRpcRequest &request)
{
    if (!light)
    {
        throw InvalidParamsException("Failed to extract light properties");
    }
    auto id = lights.addLight(std::move(light));
    request.reply(Json::serialize(id));
}

AddLightDirectionalEntrypoint::AddLightDirectionalEntrypoint(LightManager &lights)
    : AddLightEntrypoint(lights)
{
}

std::string AddLightDirectionalEntrypoint::getMethod() const
{
    return "add-light-directional";
}

std::string AddLightDirectionalEntrypoint::getDescription() const
{
    return "Add a directional light and return its ID";
}

AddLightSphereEntrypoint::AddLightSphereEntrypoint(LightManager &lights)
    : AddLightEntrypoint(lights)
{
}

std::string AddLightSphereEntrypoint::getMethod() const
{
    return "add-light-sphere";
}

std::string AddLightSphereEntrypoint::getDescription() const
{
    return "Add a sphere light and return its ID";
}

AddLightQuadEntrypoint::AddLightQuadEntrypoint(LightManager &lights)
    : AddLightEntrypoint(lights)
{
}

std::string AddLightQuadEntrypoint::getMethod() const
{
    return "add-light-quad";
}

std::string AddLightQuadEntrypoint::getDescription() const
{
    return "Add a quad light and return its ID";
}

AddLightSpotEntrypoint::AddLightSpotEntrypoint(LightManager &lights)
    : AddLightEntrypoint(lights)
{
}

std::string AddLightSpotEntrypoint::getMethod() const
{
    return "add-light-spot";
}

std::string AddLightSpotEntrypoint::getDescription() const
{
    return "Add a spot light and return its ID";
}

AddLightAmbientEntrypoint::AddLightAmbientEntrypoint(LightManager &lights)
    : AddLightEntrypoint(lights)
{
}

std::string AddLightAmbientEntrypoint::getMethod() const
{
    return "add-light-ambient";
}

std::string AddLightAmbientEntrypoint::getDescription() const
{
    return "Add an ambient light and return its ID";
}
} // namespace brayns