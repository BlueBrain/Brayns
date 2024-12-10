/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#pragma once

#include <vector>

#include <brayns/core/engine/Object.h>
#include <brayns/core/manager/ObjectManager.h>

namespace brayns
{
template<typename T>
std::vector<Stored<T>> getStoredObjects(const ObjectManager &objects, const std::vector<ObjectId> &ids)
{
    auto result = std::vector<Stored<T>>();
    result.reserve(ids.size());

    for (auto id : ids)
    {
        auto stored = objects.getAsStored<T>(id);
        result.push_back(std::move(stored));
    }

    return result;
}

template<typename T>
auto mapObjects(const std::vector<Stored<T>> &objects, auto operation)
{
    auto result = std::vector<decltype(operation(std::declval<const Stored<T> &>()))>();
    result.reserve(objects.size());

    for (const auto &object : objects)
    {
        result.push_back(operation(object));
    }

    return result;
}

template<typename T>
std::vector<ObjectId> getObjectIds(const std::vector<Stored<T>> &objects)
{
    return mapObjects(objects, [](const auto &object) { return object.getId(); });
}
}
