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
struct Metadatas
{
    std::vector<Metadata> objects;
};

template<>
struct JsonObjectReflector<Metadatas>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<Metadatas>();
        builder.field("objects", [](auto &object) { return &object.objects; })
            .description("List of object generic properties");
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

Metadatas getAllObjects(LockedObjects &locked)

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

using EmptyObject = UserObject<EmptyJson>;

template<>
struct ObjectReflector<EmptyObject>
{
    using Settings = EmptyJson;

    static std::string getType()
    {
        return "empty-object";
    }

    static EmptyJson getProperties(const EmptyObject &)
    {
        return {};
    }
};

LockedObjects::LockedObjects(ObjectManager objects, Logger &logger):
    _objects(std::move(objects)),
    _logger(&logger)
{
}

void addDefaultObjects(ObjectManager &objects)
{
    objects.addFactory<EmptyObject>([](auto, auto) { return EmptyJson(); });
}

void addObjectEndpoints(ApiBuilder &builder, LockedObjects &locked)
{
    builder.endpoint("get-all-objects", [&] { return getAllObjects(locked); })
        .description("Get generic properties of all objects, use get-{type} to get details of an object");

    builder.endpoint("get-object", [&](ObjectIdParams params) { return getObject(locked, params.id); })
        .description("Get generic object properties from given object IDs");

    builder.endpoint("update-object", [&](UpdateObjectParams params) { return updateObject(locked, params); });

    builder.endpoint("remove-objects", [&](ObjectIds params) { removeSelectedObjects(locked, params.ids); })
        .description("Remove selected objects from registry (but not from scene)");

    builder.endpoint("clear-objects", [&] { clearObjects(locked); })
        .description("Remove all objects currently in registry");

    addCreateAndGet<EmptyObject>(builder, locked);
}
}
