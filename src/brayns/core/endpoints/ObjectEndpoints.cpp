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
struct GetAllObjectsResult
{
    std::vector<ObjectSummary> objects;
};

template<>
struct JsonObjectReflector<GetAllObjectsResult>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<GetAllObjectsResult>();
        builder.field("objects", [](auto &object) { return &object.objects; }).description("List of object info");
        return builder.build();
    }
};

struct RemoveObjectsParams
{
    std::vector<ObjectId> ids;
};

template<>
struct JsonObjectReflector<RemoveObjectsParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<RemoveObjectsParams>();
        builder.field("ids", [](auto &object) { return &object.ids; }).description("IDs of the objects to remove");
        return builder.build();
    }
};

void removeObjects(ObjectManager &objects, const RemoveObjectsParams &params)
{
    for (auto id : params.ids)
    {
        objects.remove(id);
    }
}

struct EmptyObject
{
};

CreateObjectResult createEmptyObject(ObjectManager &objects, CreateObjectParams params)
{
    auto object = objects.add(EmptyObject(), {"EmptyObject"}, std::move(params));
    return getResult(object);
}

void addObjectEndpoints(ApiBuilder &builder, ObjectManager &objects)
{
    builder.endpoint("getAllObjects", [&] { return GetAllObjectsResult{objects.getAll()}; })
        .description("Get summary all objects in registry, use get-{type} to get details about a specific object");

    builder.endpoint("getObject", [&](GetObjectParams params) { return objects.get(params.id); })
        .description("Get generic object properties from given object IDs");

    builder.endpoint("updateObject", [&](UpdateObjectParams params) { objects.update(std::move(params)); })
        .description("Update generic properties of an object");

    builder.endpoint("removeObjects", [&](RemoveObjectsParams params) { removeObjects(objects, params); })
        .description("Remove selected objects from registry (but not from scene)");

    builder.endpoint("clearObjects", [&] { objects.clear(); }).description("Remove all objects currently in registry");

    builder.endpoint("createEmptyObject", [&](CreateObjectParams params) { return createEmptyObject(objects, std::move(params)); })
        .description("Create an empty object (for testing or to store user data)");
}
}
