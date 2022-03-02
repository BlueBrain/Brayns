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
AddLightAmbientEntrypoint::AddLightAmbientEntrypoint(Scene &scene)
    : AddLightEntrypoint(scene)
{
}

std::string AddLightAmbientEntrypoint::getMethod() const
{
    return "add-light-ambient";
}

std::string AddLightAmbientEntrypoint::getDescription() const
{
    return "Add an ambient light which iluminates all the scen from all directions";
}

AddLightDirectionalEntrypoint::AddLightDirectionalEntrypoint(Scene &scene)
    : AddLightEntrypoint(scene)
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

AddLightQuadEntrypoint::AddLightQuadEntrypoint(Scene &scene)
    : AddLightEntrypoint(scene)
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
} // namespace brayns
