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

auto getStorageIterator(auto &objects, ObjectId id)
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

std::vector<Metadata> ObjectManager::getAllMetadata() const
{
    auto metadatas = std::vector<Metadata>();
    metadatas.reserve(_objects.size());

    for (const auto &[id, object] : _objects)
    {
        auto metadata = createObjectMetadata(object);
        metadatas.push_back(std::move(metadata));
    }

    return metadatas;
}

Metadata ObjectManager::getMetadata(ObjectId id) const
{
    const auto &interface = getInterface(id);

    return createObjectMetadata(interface);
}

void ObjectManager::setUserData(ObjectId id, const JsonValue &userData)
{
    auto i = getStorageIterator(_objects, id);

    i->second.setUserData(userData);
}

void ObjectManager::remove(ObjectId id)
{
    auto i = getStorageIterator(_objects, id);

    i->second.remove();

    _objects.erase(i);

    _ids.recycle(id);
}

void ObjectManager::clear()
{
    for (const auto &[id, object] : _objects)
    {
        object.remove();
    }

    _objects.clear();

    _ids = {};
    disableNullId(_ids);
}

const ObjectInterface &ObjectManager::getInterface(ObjectId id) const
{
    auto i = getStorageIterator(_objects, id);

    return i->second;
}
}
