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

#include "GetClipPlanesEntrypoint.h"

#include <brayns/engine/defaultcomponents/ClippingComponent.h>
#include <brayns/engine/geometries/Plane.h>

namespace brayns
{
GetClipPlanesEntrypoint::GetClipPlanesEntrypoint(Scene &scene)
    : _scene(scene)
{
}

std::string GetClipPlanesEntrypoint::getMethod() const
{
    return "get-clip-planes";
}

std::string GetClipPlanesEntrypoint::getDescription() const
{
    return "Get all clip planes";
}

void GetClipPlanesEntrypoint::onRequest(const Request &request)
{
    // Right now we only allow clipping planes, so we can safely assure that any clipping model uses plane geometry
    auto& clippingModels = _scene.getAllClippingModels();

    std::vector<ClipPlane> result;
    result.reserve(clippingModels.size());

    for(auto& [id, model] : clippingModels)
    {
        auto& clipComponent = model->getComponent<ClippingComponent<Plane>>();
        auto& geometry = clipComponent.getGeometry();
        auto& plane = geometry.get(0);

        result.emplace_back(id, plane.coefficents);
    }

    request.reply(result);
}
} // namespace brayns
