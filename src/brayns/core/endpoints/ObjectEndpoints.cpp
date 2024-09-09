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
        builder.field("objects", [](auto &object) { return &object.objects; }).description("List of object info");
        return builder.build();
    }
};

AllObjectsResult getAllObjects(ObjectManager &manager)
{
    return {manager.visit([](auto &objects) { return objects.getAll(); })};
}

ObjectInfo getObject(ObjectManager &manager, const ObjectParams &params)
{
    return manager.visit([&](auto &objects) { return objects.get(params.id); });
}

struct ObjectUpdate
{
    JsonValue userData;
};

template<>
struct JsonObjectReflector<ObjectUpdate>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<ObjectUpdate>();
        builder.field("userData", [](auto &object) { return &object.userData; })
            .description("User data to store in the object (not used by Brayns)");
        return builder.build();
    }
};

void updateObject(ObjectManager &manager, const UpdateParams<ObjectUpdate> &params)
{
    manager.visit([&](auto &objects) { objects.update(params.id, params.settings.userData); });
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

void removeObjects(ObjectManager &manager, const RemoveParams &params)
{
    manager.visit(
        [&](auto &objects)
        {
            for (auto id : params.ids)
            {
                objects.remove(id);
            }
        });
}

void clearObjects(ObjectManager &manager)
{
    manager.visit([](auto &objects) { objects.clear(); });
}

struct EmptyObject
{
};

ObjectResult createEmptyObject(ObjectManager &manager)
{
    return manager.visit(
        [&](auto &objects)
        {
            auto object = objects.add(EmptyObject(), "EmptyObject");
            return ObjectResult{object.getId()};
        });
}

void addObjectEndpoints(ApiBuilder &builder, ObjectManager &manager)
{
    builder.endpoint("getAllObjects", [&] { return getAllObjects(manager); })
        .description("Get generic properties of all objects, use get-{type} to get details of an object");

    builder.endpoint("getObject", [&](ObjectParams params) { return getObject(manager, params); })
        .description("Get generic object properties from given object IDs");

    builder.endpoint("updateObject", [&](UpdateParams<ObjectUpdate> params) { return updateObject(manager, params); });

    builder.endpoint("removeObjects", [&](RemoveParams params) { removeObjects(manager, params); })
        .description("Remove selected objects from registry (but not from scene)");

    builder.endpoint("clearObjects", [&] { clearObjects(manager); })
        .description("Remove all objects currently in registry");

    builder.endpoint("createEmptyObject", [&] { return createEmptyObject(manager); })
        .description("Create an empty object (for testing or to store user data)");
}
}
