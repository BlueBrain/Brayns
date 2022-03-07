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

#include "GetLightsEntrypoint.h"

namespace brayns
{
GetLightsEntrypoint::GetLightsEntrypoint(Scene &scene, LightFactory::Ptr lightFactory)
    : _scene(scene)
    , _lightFactory(lightFactory)
{
}

std::string GetLightsEntrypoint::getMethod() const
{
    return "get-lights";
}

std::string GetLightsEntrypoint::getDescription() const
{
    return "Get all lights";
}

void GetLightsEntrypoint::onRequest(const Request &request)
{
    auto &lights = _scene.getAllLights();

    std::vector<GenericLight> result;
    result.reserve(lights.size());

    for(const auto& [id, light] : lights)
    {
        result.emplace_back();
        auto& genericLight = result.back();
        genericLight.serialize(*_lightFactory, *light);
        genericLight.setID(id);
    }

    request.reply(result);
}
} // namespace brayns
