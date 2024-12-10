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

#include "ObjectManager.h"

#include <cassert>

#include <brayns/core/jsonrpc/Errors.h>

namespace
{
using namespace brayns;

auto getIterator(auto &objects, ObjectId id)
{
    auto i = objects.find(id);

    if (i == objects.end())
    {
        throw InvalidParams(fmt::format("No objects found with ID {}", id));
    }

    return i;
}
}

namespace brayns
{
std::vector<ObjectSummary> ObjectManager::getAll() const
{
    auto objects = std::vector<ObjectSummary>();
    objects.reserve(_objects.size());

    for (const auto &[id, object] : _objects)
    {
        auto summary = getSummary(*object);
        objects.push_back(std::move(summary));
    }

    return objects;
}

GetObjectResult ObjectManager::get(ObjectId id) const
{
    const auto &object = retreive(id);
    return getResult(*object);
}

void ObjectManager::update(UpdateObjectParams params)
{
    const auto &object = retreive(params.id);
    object->settings = getUpdatedParams(params, object->settings);
}

void ObjectManager::remove(ObjectId id)
{
    auto i = getIterator(_objects, id);

    i->second->id = nullId;

    _objects.erase(i);
    _ids.recycle(id);
}

void ObjectManager::clear()
{
    for (const auto &[id, object] : _objects)
    {
        object->id = nullId;
    }

    _objects.clear();
    _ids.reset();
}

void ObjectManager::store(std::shared_ptr<UserObject> object)
{
    auto id = _ids.next();

    try
    {
        auto [i, inserted] = _objects.emplace(id, std::move(object));
        assert(inserted);
        i->second->id = id;
    }
    catch (...)
    {
        _ids.recycle(id);
        throw;
    }
}

const std::shared_ptr<UserObject> &ObjectManager::retreive(ObjectId id) const
{
    auto i = getIterator(_objects, id);
    return i->second;
}
}
