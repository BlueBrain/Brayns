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
struct AllObjectsResult
{
    std::vector<ObjectInfo> objects;
};

template<>
struct JsonObjectReflector<AllObjectsResult>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<AllObjectsResult>();
        builder.field("objects", [](auto &object) { return &object.objects; })
            .description("Generic properties of all objects in registry");
        return builder.build();
    }
};

AllObjectsResult getAllObjects(LockedObjects &locked)
{
    return {locked.visit([](auto &objects) { return objects.getAllObjects(); })};
}

ObjectInfo getObject(LockedObjects &locked, const ObjectParams &params)
{
    return locked.visit([&](auto &objects) { return objects.getObject(params.id); });
}

struct UserProperties
{
    JsonValue userData;
};

template<>
struct JsonObjectReflector<UserProperties>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<UserProperties>();
        builder.field("user_data", [](auto &object) { return &object.userData; }).description("User data");
        return builder.build();
    }
};

using ObjectUpdate = UpdateParams<UserProperties>;

void updateObject(LockedObjects &locked, const ObjectUpdate &params)
{
    locked.visit([&](auto &objects) { objects.setUserData(params.id, params.properties.userData); });
}

struct RemoveParams
{
    std::vector<ObjectId> ids;
};

template<>
struct JsonObjectReflector<RemoveParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<RemoveParams>();
        builder.field("ids", [](auto &object) { return &object.ids; }).description("IDs of the objects to remove");
        return builder.build();
    }
};

void removeObjects(LockedObjects &locked, const RemoveParams &params)
{
    locked.visit(
        [&](auto &objects)
        {
            for (auto id : params.ids)
            {
                objects.remove(id);
            }
        });
}

void clearObjects(LockedObjects &locked)
{
    locked.visit([](auto &objects) { objects.clear(); });
}

struct EmptyObject
{
};

template<>
struct ObjectReflector<EmptyObject>
{
    static std::string getType(const EmptyObject &)
    {
        return "empty-object";
    }
};

ObjectResult createEmptyObject(LockedObjects &locked)
{
    return locked.visit(
        [&](auto &objects)
        {
            auto object = objects.add(EmptyObject());
            return object.getResult();
        });
}

void addObjectEndpoints(ApiBuilder &builder, LockedObjects &objects)
{
    builder.endpoint("get-all-objects", [&] { return getAllObjects(objects); })
        .description("Get generic properties of all objects, use get-{type} to get details of an object");

    builder.endpoint("get-object", [&](ObjectParams params) { return getObject(objects, params); })
        .description("Get generic object properties from given object IDs");

    builder.endpoint("update-object", [&](ObjectUpdate params) { return updateObject(objects, params); });

    builder.endpoint("remove-objects", [&](RemoveParams params) { removeObjects(objects, params); })
        .description("Remove selected objects from registry (but not from scene)");

    builder.endpoint("clear-objects", [&] { clearObjects(objects); })
        .description("Remove all objects currently in registry");

    builder.endpoint("create-empty-object", [&] { return createEmptyObject(objects); })
        .description("Create an empty object (for testing or to store user data)");
}
}
