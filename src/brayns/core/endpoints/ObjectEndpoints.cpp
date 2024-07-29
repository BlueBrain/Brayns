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

#include "ObjectEndpoints.h"

namespace brayns
{
struct TagList
{
    std::vector<std::string> tags;
};

template<>
struct JsonObjectReflector<TagList>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<TagList>();
        builder.field("tags", [](auto &object) { return &object.tags; }).description("List of object tag");
        return builder.build();
    }
};

struct MetadataList
{
    std::vector<Metadata> objects;
};

template<>
struct JsonObjectReflector<MetadataList>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<MetadataList>();
        builder.field("objects", [](auto &object) { return &object.objects; })
            .description("List of object generic properties");
        return builder.build();
    }
};

std::vector<Metadata> getMetadata(ObjectManager &objects, const std::vector<ObjectId> &ids)
{
    auto metadatas = std::vector<Metadata>();
    metadatas.reserve(ids.size());

    for (auto id : ids)
    {
        const auto &metadata = objects.getMetadata(id);
        metadatas.push_back(metadata);
    }

    return metadatas;
}

std::vector<ObjectId> getIdsFromTags(ObjectManager &objects, const std::vector<std::string> &tags)
{
    auto ids = std::vector<ObjectId>();
    ids.reserve(tags.size());

    for (const auto &tag : tags)
    {
        auto id = objects.getId(tag);
        ids.push_back(id);
    }

    return ids;
}

void removeObjects(ObjectManager &objects, const std::vector<ObjectId> &ids)
{
    for (auto id : ids)
    {
        objects.remove(id);
    }
}

void addObjectEndpoints(ApiBuilder &builder, ObjectManager &objects)
{
    builder.endpoint("get-all-objects", [&] { return MetadataList{objects.getAllMetadata()}; })
        .description("Return the generic properties of all objects, use get-{type} to get specific properties");

    builder.endpoint("get-objects", [&](IdList params) { return MetadataList{getMetadata(objects, params.ids)}; })
        .description("Get generic object properties from given object IDs");

    builder.endpoint("get-object-ids", [&](TagList params) { return IdList{getIdsFromTags(objects, params.tags)}; })
        .description("Map given list of tags to object IDs (result is an array in the same order as params)");

    builder.endpoint("remove-objects", [&](IdList params) { removeObjects(objects, params.ids); })
        .description(
            "Remove objects from the registry, the ID can be reused by future objects. Note that the object can stay "
            "in memory as long as it is used by other objects (using a ref-counted system)");

    builder.endpoint("clear-objects", [&] { objects.clear(); }).description("Remove all objects currently in registry");
}
}
