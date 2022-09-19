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

#include <brayns/engine/components/Clippers.h>
#include <brayns/engine/systems/ClipperInitSystem.h>

namespace brayns
{
AddClipPlaneEntrypoint::AddClipPlaneEntrypoint(ModelManager &models)
    : _models(models)
{
}

std::string AddClipPlaneEntrypoint::getMethod() const
{
    return "add-clip-plane";
}

std::string AddClipPlaneEntrypoint::getDescription() const
{
    return "Add a clip plane and returns the clip plane ID";
}

void AddClipPlaneEntrypoint::onRequest(const Request &request)
{
    auto plane = request.getParams();
    auto model = std::make_unique<Model>();

    auto &components = model->getComponents();
    auto &clippers = components.add<Clippers>();
    clippers.elements.emplace_back(plane);

    auto &systems = model->getSystems();
    systems.setInitSystem<ClipperInitSystem>();

    auto *instance = _models.addModel(std::move(model));
    request.reply(*instance);
}
} // namespace brayns
