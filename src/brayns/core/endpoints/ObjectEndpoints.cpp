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
    std::vector<Metadata> objects;
};

template<>
struct JsonObjectReflector<AllObjectsResult>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<AllObjectsResult>();
        builder.field("objects", [](auto &object) { return &object.objects; })
            .description("Metadata of all objects in registry");
        return builder.build();
    }
};

struct UpdateObjectParams
{
    ObjectId id;
    JsonValue userData;
};

template<>
struct JsonObjectReflector<UpdateObjectParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<UpdateObjectParams>();
        builder.field("id", [](auto &object) { return &object.id; }).description("ID of the object to update");
        builder.field("user_data", [](auto &object) { return &object.userData; })
            .description("New user data to store in the object");
        return builder.build();
    }
};

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

AllObjectsResult getAllObjects(LockedObjects &locked)
{
    return {locked.visit([](auto &objects) { return objects.getAllMetadata(); })};
}

Metadata getObject(LockedObjects &locked, ObjectId id)
{
    return locked.visit([&](auto &objects) { return objects.getMetadata(id); });
}

Metadata updateObject(LockedObjects &locked, const UpdateObjectParams &params)
{
    return locked.visit(
        [&](auto &objects)
        {
            objects.setUserData(params.id, params.userData);
            return objects.getMetadata(params.id);
        });
}

void removeSelectedObjects(LockedObjects &locked, const std::vector<ObjectId> &ids)
{
    locked.visit(
        [&](auto &objects)
        {
            for (auto id : ids)
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
    using Settings = EmptyJson;

    static std::string getType(const EmptyObject &)
    {
        return "empty-object";
    }

    static EmptyJson getProperties(const EmptyObject &)
    {
        return {};
    }
};

void addDefaultObjects(ObjectManager &objects)
{
    objects.addFactory<EmptyObject>([](auto, auto) { return EmptyObject(); });
}

void addObjectEndpoints(ApiBuilder &builder, LockedObjects &objects)
{
    builder.endpoint("get-all-objects", [&] { return getAllObjects(objects); })
        .description("Get generic properties of all objects, use get-{type} to get details of an object");

    builder.endpoint("get-object", [&](GetObjectParams params) { return getObject(objects, params.id); })
        .description("Get generic object properties from given object IDs");

    builder.endpoint("update-object", [&](UpdateObjectParams params) { return updateObject(objects, params); });

    builder.endpoint("remove-objects", [&](RemoveParams params) { removeSelectedObjects(objects, params.ids); })
        .description("Remove selected objects from registry (but not from scene)");

    builder.endpoint("clear-objects", [&] { clearObjects(objects); })
        .description("Remove all objects currently in registry");

    builder
        .endpoint(
            "create-empty-object",
            [&](ParamsOf<EmptyObject> params) { return objects.create<EmptyObject>(params); })
        .description("Create an empty object with only metadata");

    builder.endpoint("get-empty-object", [&](GetObjectParams params) { return objects.get<EmptyObject>(params); })
        .description("Get empty object with given ID");
}
}
