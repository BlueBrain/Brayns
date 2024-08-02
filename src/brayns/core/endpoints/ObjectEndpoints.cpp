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

void removeSelectedObjects(ObjectManager &objects, const std::vector<ObjectId> &ids)
{
    for (auto id : ids)
    {
        objects.remove(id);
    }
}

using EmptyObject = UserObject<std::optional<EmptyJsonObject>>;
using EmptyObjectParams = UserObjectParams<std::optional<EmptyJsonObject>>;

EmptyObject &createEmptyObject(ObjectManager &objects, const EmptyObjectParams &params)
{
    return objects.create<EmptyObject>({"object", {}, params.userData});
}

void addObjectEndpoints(ApiBuilder &builder, ObjectManager &objects)
{
    builder.endpoint("get-all-objects", [&] { return ObjectList{objects.getAllMetadata()}; })
        .description("Return the generic properties of all objects, use get-{type} to get specific properties");

    builder.endpoint("get-object", [&](ObjectIdParams params) { return objects.getMetadata(params.id); })
        .description("Get generic object properties from given object IDs");

    builder.endpoint("remove-objects", [&](ObjectIds params) { removeSelectedObjects(objects, params.ids); })
        .description("Remove selected objects from registry (but not from scene)");

    builder.endpoint("clear-objects", [&] { objects.clear(); }).description("Remove all objects currently in registry");

    builder
        .endpoint("create-empty-object", [&](EmptyObjectParams params) { return createEmptyObject(objects, params); })
        .description("Create an empty object just to store user data");

    builder.endpoint("get-empty-object", [&](ObjectIdParams params) { return objects.get<EmptyObject>(params.id); })
        .description("Create an empty object just to store user data");
}
}
