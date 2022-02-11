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

#include "GetInstancesEntrypoint.h"

#include <algorithm>

#include <brayns/network/common/ExtractModel.h>

namespace brayns
{
GetInstancesEntrypoint::GetInstancesEntrypoint(Scene &scene)
    : _scene(scene)
{
}

std::string GetInstancesEntrypoint::getName() const
{
    return "get-instances";
}

std::string GetInstancesEntrypoint::getDescription() const
{
    return "Get instances of the given model";
}

void GetInstancesEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    auto id = params.id;
    auto &range = params.result_range;
    auto &model = ExtractModel::fromId(_scene, id);
    auto &instances = model.getInstances();
    auto first = instances.begin();
    auto from = std::min(size_t(range.x), instances.size());
    auto to = std::min(size_t(range.y), instances.size());
    request.reply({first + from, first + to});
}
} // namespace brayns
