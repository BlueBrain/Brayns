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
struct ObjectList
{
    std::vector<Metadata> objects;
};

template<>
struct JsonObjectReflector<ObjectList>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<ObjectList>();
        builder.field("objects", [](auto &object) { return &object.objects; })
            .description("List of object generic properties");
        return builder.build();
    }
};

std::vector<Metadata> getSelectedObjects(ObjectManager &objects, const std::vector<ObjectId> &ids)
{
    auto metadatas = std::vector<Metadata>();
    metadatas.reserve(ids.size());

    for (auto id : ids)
    {
        const auto &metadata = objects.getObject(id);
        metadatas.push_back(metadata);
    }

    return metadatas;
}

void removeSelectedObjects(ObjectManager &objects, const std::vector<ObjectId> &ids)
{
    for (auto id : ids)
    {
        objects.remove(id);
    }
}

void addObjectEndpoints(ApiBuilder &builder, ObjectManager &objects)
{
    builder.endpoint("get-all-objects", [&] { return ObjectList{objects.getAllObjects()}; })
        .description("Return the generic properties of all objects, use get-{type} to get specific properties");

    builder.endpoint("get-objects", [&](IdList params) { return ObjectList{getSelectedObjects(objects, params.ids)}; })
        .description("Get generic object properties from given object IDs");

    builder.endpoint("remove-objects", [&](IdList params) { removeSelectedObjects(objects, params.ids); })
        .description(
            "Remove objects from the registry, the ID can be reused by future objects. Note that the object can stay "
            "in memory as long as it is used by other objects (using a ref-counted system)");

    builder.endpoint("clear-objects", [&] { objects.clear(); }).description("Remove all objects currently in registry");
}
}
