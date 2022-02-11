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

#include "AddClipPlaneEntrypoint.h"

namespace brayns
{
AddClipPlaneEntrypoint::AddClipPlaneEntrypoint(Engine &engine, INetworkInterface &interface)
    : _engine(engine)
    , _notifier(interface)
{
}

std::string AddClipPlaneEntrypoint::getName() const
{
    return "add-clip-plane";
}

std::string AddClipPlaneEntrypoint::getDescription() const
{
    return "Add a clip plane and returns the clip plane descriptor";
}

JsonSchema AddClipPlaneEntrypoint::getParamsSchema() const
{
    auto schema = Json::getSchema<ClipPlane>();
    schema.properties.erase("id");
    return schema;
}

void AddClipPlaneEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    auto &plane = params.plane;
    auto &scene = _engine.getScene();
    auto id = scene.addClipPlane(plane);
    auto clipPlane = scene.getClipPlane(id);
    _engine.triggerRender();
    _notifier.notify(request, clipPlane);
    request.reply(clipPlane);
}
} // namespace brayns
