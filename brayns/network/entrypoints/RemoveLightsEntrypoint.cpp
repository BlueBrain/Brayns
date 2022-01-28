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

#include "RemoveLightsEntrypoint.h"

namespace brayns
{
std::string RemoveLightsEntrypoint::getName() const
{
    return "remove-lights";
}

std::string RemoveLightsEntrypoint::getDescription() const
{
    return "Remove the model(s) from the ID list from the scene";
}

void RemoveLightsEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    auto &ids = params.ids;
    auto &engine = getApi().getEngine();
    auto &scene = engine.getScene();
    auto &lightManager = scene.getLightManager();
    for (auto id : ids)
    {
        lightManager.removeLight(id);
    }
    triggerRender();
    request.reply(EmptyMessage());
}
} // namespace brayns
