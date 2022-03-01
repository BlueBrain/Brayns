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

#include <brayns/network/entrypoints/AddClipPlaneEntrypoint.h>

#include <brayns/engine/models/ClippingModel.h>

namespace brayns
{
AddClipPlaneEntrypoint::AddClipPlaneEntrypoint(Scene &scene)
    : _scene(scene)
{
}

std::string AddClipPlaneEntrypoint::getMethod() const
{
    return "add-clip-plane";
}

std::string AddClipPlaneEntrypoint::getDescription() const
{
    return "Add a clip plane and returns the clip plane descriptor";
}

void AddClipPlaneEntrypoint::onRequest(const Request &request)
{
    auto plane = request.getParams();

    auto model = std::make_unique<ClippingModel<Plane>>();
    model->addClipGeometry(plane);
    auto modelPtr = model.get();
    auto id = _scene.addClippingModel(std::move(model));

    AddClipGeometryResultMessage result;
    result.id = id;
    result.geometry = modelPtr->serializeGeometry();

    request.reply(result);
}
} // namespace brayns
