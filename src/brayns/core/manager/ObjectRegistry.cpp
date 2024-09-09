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

#include "ObjectRegistry.h"

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
void ObjectRegistry::remove(ObjectId id)
{
    auto i = getIterator(_objects, id);

    i->second->info.id = nullId;

    _objects.erase(i);
    _ids.recycle(id);
}

void ObjectRegistry::clear()
{
    for (const auto &[id, object] : _objects)
    {
        object->info.id = nullId;
    }

    _objects.clear();
    _ids.reset();
}

ObjectInfo ObjectRegistry::get(ObjectId id) const
{
    const auto &object = retreive(id);
    return object->info;
}

std::vector<ObjectInfo> ObjectRegistry::getAll() const
{
    auto objects = std::vector<ObjectInfo>();
    objects.reserve(_objects.size());

    for (const auto &[id, object] : _objects)
    {
        objects.push_back(object->info);
    }

    return objects;
}

void ObjectRegistry::update(ObjectId id, const JsonValue &userData)
{
    auto i = getIterator(_objects, id);
    i->second->info.userData = userData;
}

void ObjectRegistry::store(std::shared_ptr<UserObject> object)
{
    auto id = _ids.next();

    try
    {
        object->info.id = id;
        _objects[id] = std::move(object);
    }
    catch (...)
    {
        _ids.recycle(id);
        throw;
    }
}

const std::shared_ptr<UserObject> &ObjectRegistry::retreive(ObjectId id) const
{
    auto i = getIterator(_objects, id);
    return i->second;
}
}
