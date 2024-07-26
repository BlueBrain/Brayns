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

#include <fmt/format.h>

namespace
{
using namespace brayns;

using IdByTag = std::unordered_map<std::string, ObjectId>;

void disableNullId(IdGenerator<ObjectId> &ids)
{
    ids.next();
}

void checkTagIsNotAlreadyUsed(const IdByTag &ids, const std::string &tag)
{
    auto i = ids.find(tag);

    if (i == ids.end())
    {
        return;
    }

    throw InvalidParams(fmt::format("Tag '{}' already used by object with ID {}", tag, i->first));
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

std::vector<ObjectId> ObjectManager::getIds() const
{
    auto ids = std::vector<ObjectId>();
    ids.reserve(_objects.size());

    for (const auto &[id, object] : _objects)
    {
        ids.push_back(id);
    }

    return ids;
}

const Metadata &ObjectManager::getMetadata(ObjectId id) const
{
    auto i = getObjectIterator(_objects, id);

    return i->second.getMetadata();
}

ObjectId ObjectManager::getId(const std::string &tag) const
{
    auto i = _idsByTag.find(tag);

    if (i == _idsByTag.end())
    {
        throw InvalidParams(fmt::format("No objects found with tag '{}'", tag));
    }

    return i->second;
}

void ObjectManager::remove(ObjectId id)
{
    auto i = getObjectIterator(_objects, id);

    auto &metadata = i->second.getMetadata();

    const auto &tag = metadata.tag;

    if (!tag.empty())
    {
        auto count = _idsByTag.erase(tag);
        assert(count == 1);
    }

    metadata.id = nullId;
    _objects.erase(i);
}

InvalidParams ObjectManager::invalidType(const ObjectManagerEntry &entry)
{
    const auto &metadata = entry.getMetadata();
    auto id = metadata.type;
    const auto &type = metadata.type;

    return InvalidParams(fmt::format("Invalid type for object with ID {} ('{}')", id, type));
}

const ObjectManagerEntry &ObjectManager::getEntry(ObjectId id) const
{
    auto i = getObjectIterator(_objects, id);

    return i->second;
}

void ObjectManager::addEntry(ObjectId id, ObjectManagerEntry entry)
{
    const auto &metadata = entry.getMetadata();
    const auto &tag = metadata.tag;

    if (!tag.empty())
    {
        checkTagIsNotAlreadyUsed(_idsByTag, tag);

        auto [i, inserted] = _idsByTag.emplace(tag, id);
        assert(inserted);
    }

    try
    {
        auto [i, inserted] = _objects.emplace(id, std::move(entry));
        assert(inserted);
    }
    catch (...)
    {
        _idsByTag.erase(tag);
        throw;
    }
}
}
