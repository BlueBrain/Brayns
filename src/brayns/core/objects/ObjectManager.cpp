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

#include <fmt/format.h>

namespace
{
using namespace brayns;

void disableNullId(IdGenerator<ObjectId> &ids)
{
    ids.next();
}

auto getObjectIterator(auto &objects, ObjectId id)
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

ObjectManager::ObjectManager()
{
    disableNullId(_ids);
}

std::vector<Metadata> ObjectManager::getAllObjects() const
{
    auto objects = std::vector<Metadata>();
    objects.reserve(_objects.size());

    for (const auto &[id, object] : _objects)
    {
        objects.push_back(*object.getMetadata());
    }

    return objects;
}

const Metadata &ObjectManager::getObject(ObjectId id) const
{
    auto i = getObjectIterator(_objects, id);
    return *i->second.getMetadata();
}

void ObjectManager::remove(ObjectId id)
{
    auto i = getObjectIterator(_objects, id);

    auto &metadata = *i->second.getMetadata();

    metadata.id = nullId;
    _objects.erase(i);
}

void ObjectManager::clear()
{
    _objects.clear();
    _idsByTag.clear();
    _ids = {};
    disableNullId(_ids);
}

void ObjectManager::checkType(const ObjectManagerEntry &entry, const std::type_info &expected)
{
    if (entry.object.type() == expected)
    {
        return;
    }

    const auto &metadata = *entry.getMetadata();
    auto id = metadata.id;
    const auto &type = metadata.type;

    throw InvalidParams(fmt::format("Invalid type '{}' for object with ID {}", id, type));
}

const ObjectManagerEntry &ObjectManager::getEntry(ObjectId id) const
{
    auto i = getObjectIterator(_objects, id);
    return i->second;
}
}
