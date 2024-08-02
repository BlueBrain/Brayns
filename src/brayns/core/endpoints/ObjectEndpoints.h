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

#pragma once

#include <brayns/core/api/ApiBuilder.h>
#include <brayns/core/objects/ObjectManager.h>

namespace brayns
{
struct ObjectIds
{
    std::vector<ObjectId> ids;
};

template<>
struct JsonObjectReflector<ObjectIds>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<ObjectIds>();
        builder.field("ids", [](auto &object) { return &object.ids; }).description("List of object IDs");
        return builder.build();
    }
};

struct ObjectIdParams
{
    ObjectId id;
};

template<>
struct JsonObjectReflector<ObjectIdParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<ObjectIdParams>();
        builder.field("id", [](auto &object) { return &object.id; }).description("Objects ID");
        return builder.build();
    }
};

struct EmptyJsonObject
{
};

template<>
struct JsonObjectReflector<EmptyJsonObject>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<EmptyJsonObject>();
        builder.description("Placeholder empty object");
        return builder.build();
    }
};

template<ReflectedJson T>
struct UserObjectParams
{
    T settings;
    JsonValue userData;
};

template<ReflectedJson T>
struct JsonObjectReflector<UserObjectParams<T>>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<UserObjectParams<T>>();
        builder.field("settings", [](auto &object) { return &object.settings; })
            .description("Settings specific to object type");
        builder.field("user_data", [](auto &object) { return &object.userData; })
            .description("User data to attach to the object (not used by Brayns)");
        return builder.build();
    }
};

void addObjectEndpoints(ApiBuilder &builder, ObjectManager &objects);
}
