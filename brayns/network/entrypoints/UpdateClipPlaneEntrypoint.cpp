/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
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

#include "UpdateClipPlaneEntrypoint.h"

#include <brayns/network/entrypoint/EntrypointException.h>

namespace brayns
{
std::string UpdateClipPlaneEntrypoint::getName() const
{
    return "update-clip-plane";
}

std::string UpdateClipPlaneEntrypoint::getDescription() const
{
    return "Update a clip plane with the given coefficients";
}

void UpdateClipPlaneEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    auto id = params.getID();
    auto &plane = params.getPlane();
    auto &engine = getApi().getEngine();
    auto &scene = engine.getScene();
    auto clipPlane = scene.getClipPlane(id);
    if (!clipPlane)
    {
        throw EntrypointException("No clip plane found with ID " + std::to_string(id));
    }
    clipPlane->setPlane(plane);
    engine.triggerRender();
    request.notify(clipPlane);
    request.reply(EmptyMessage());
}
} // namespace brayns
