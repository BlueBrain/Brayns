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
struct TagParams
{
    std::string tag;
};

template<>
struct JsonObjectReflector<TagParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<TagParams>();
        builder.field("tag", [](auto &object) { return &object.tag; }).description("Object tag set by user");
        return builder.build();
    }
};

struct ObjectsResult
{
    std::vector<Metadata> objects;
};

template<>
struct JsonObjectReflector<ObjectsResult>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<ObjectsResult>();
        builder.field("objects", [](auto &object) { return &object.objects; })
            .description("Generic properties of all objects in registry, use get-{type} to get specific properties");
        return builder.build();
    }
};

struct IdParams
{
    ObjectId id;
};

template<>
struct JsonObjectReflector<IdParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<IdParams>();
        builder.field("id", [](auto &object) { return &object.id; })
            .description("Object unique ID generated at object creation");
        return builder.build();
    }
};

void addObjectEndpoints(ApiBuilder &builder, ObjectManager &objects)
{
    builder.endpoint("get-objects", [&] { return ObjectsResult{objects.getAllMetadata()}; })
        .description("Return the IDs of all existing objects");

    builder.endpoint("get-object", [&](IdParams params) { return objects.getMetadata(params.id); })
        .description("Get generic object data from given object ID");

    builder.endpoint("get-object-id", [&](TagParams params) { return IdParams{objects.getId(params.tag)}; })
        .description("Get the object ID associated with the given tag set by user");

    builder.endpoint("remove-object", [&](IdParams params) { objects.remove(params.id); })
        .description(
            "Remove object with given ID from the registry, the ID can be reused by future objects. Note that the "
            "object can stay in memory as long as it is used by other objects");

    builder.endpoint("clear-objects", [&] { objects.clear(); }).description("Remove all objects currently in registry");
}
}
