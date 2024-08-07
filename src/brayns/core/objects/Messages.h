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

#include <optional>
#include <string>

#include <brayns/core/json/Json.h>

namespace brayns
{
using ObjectId = std::uint32_t;

constexpr auto nullId = ObjectId(0);

struct Metadata
{
    ObjectId id;
    std::string type;
    std::size_t size = 0;
    JsonValue userData = {};
};

template<>
struct JsonObjectReflector<Metadata>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<Metadata>();
        builder.field("id", [](auto &object) { return &object.id; })
            .description("Object ID, primary way to query this object (starts at 1)");
        builder.field("type", [](auto &object) { return &object.type; })
            .description("Object type key, use 'get-{type}' to query detailed information about the object");
        builder.field("size", [](auto &object) { return &object.size; })
            .description("Object size in memory (in bytes), 0 if negligible");
        builder.field("user_data", [](auto &object) { return &object.userData; }).description("Data set by user");
        return builder.build();
    }
};

template<ReflectedJson T>
struct ObjectParams
{
    T settings;
    JsonValue userData = {};
};

template<ReflectedJson T>
struct JsonObjectReflector<ObjectParams<T>>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<ObjectParams<T>>();
        builder.field("settings", [](auto &object) { return &object.settings; })
            .description("Settings to create the object");
        builder.field("user_data", [](auto &object) { return &object.userData; })
            .description("Optional user data (only for user, not used by Brayns)")
            .defaultValue(NullJson());
        return builder.build();
    }
};

template<ReflectedJson T>
struct ObjectResult
{
    Metadata metadata;
    T properties;
};

template<ReflectedJson T>
struct JsonObjectReflector<ObjectResult<T>>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<ObjectResult<T>>();
        builder.field("metadata", [](auto &object) { return &object.metadata; })
            .description("Generic object properties");
        builder.field("properties", [](auto &object) { return &object.properties; })
            .description("Properties that are specific to the object type");
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
        builder.description("An empty object");
        return builder.build();
    }
};

using EmptyJson = std::optional<EmptyJsonObject>;

struct GetObjectParams
{
    ObjectId id;
};

template<>
struct JsonObjectReflector<GetObjectParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<GetObjectParams>();
        builder.field("id", [](auto &object) { return &object.id; }).description("ID of the object to retreive");
        return builder.build();
    }
};
}
