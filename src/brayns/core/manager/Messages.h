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

#include <string>

#include <brayns/core/json/Json.h>

namespace brayns
{
using ObjectId = std::uint32_t;

constexpr auto nullId = ObjectId(0);

struct ObjectMetadata
{
    std::string type;
};

template<>
struct JsonObjectReflector<ObjectMetadata>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<ObjectMetadata>();
        builder.field("type", [](auto &object) { return &object.type; }).description("Object type");
        return builder.build();
    }
};

struct ObjectSettings
{
    std::string tag = {};
    JsonValue userData = {};
};

template<>
struct JsonObjectReflector<ObjectSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<ObjectSettings>();
        builder.field("tag", [](auto &object) { return &object.tag; })
            .description("Optional user tag, visible also in object summary (not used by Brayns)")
            .defaultValue("");
        builder.field("userData", [](auto &object) { return &object.userData; })
            .description("Optional user data, not visible in object summary (not used by Brayns)")
            .defaultValue(NullJson());
        return builder.build();
    }
};

struct ObjectSummary
{
    ObjectId id;
    std::string type;
    std::string tag;
};

template<>
struct JsonObjectReflector<ObjectSummary>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<ObjectSummary>();
        builder.field("id", [](auto &object) { return &object.id; })
            .description("Object ID (0 is reserved for objects that are not stored in the registry, so valid object IDs start at 1)");
        builder.field("type", [](auto &object) { return &object.type; })
            .description("Object type, use 'get{type}' to query detailed information about the object");
        builder.field("tag", [](auto &object) { return &object.tag; }).description("Optional tag set by user");
        return builder.build();
    }
};

using CreateObjectParams = ObjectSettings;

struct CreateObjectResult
{
    ObjectId id;
};

template<>
struct JsonObjectReflector<CreateObjectResult>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<CreateObjectResult>();
        builder.field("id", [](auto &object) { return &object.id; }).description("ID of the object that has been created");
        return builder.build();
    }
};

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
        builder.field("id", [](auto &object) { return &object.id; }).description("ID of the object to get");
        return builder.build();
    }
};

struct GetObjectResult
{
    ObjectMetadata metadata;
    ObjectSettings settings;
};

template<>
struct JsonObjectReflector<GetObjectResult>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<GetObjectResult>();
        builder.extend([](auto &object) { return &object.metadata; });
        builder.extend([](auto &object) { return &object.settings; });
        return builder.build();
    }
};

template<ReflectedJsonObject T, ReflectedJsonObject U>
struct ComposedParamsOf
{
    T base;
    U derived;
};

template<ReflectedJsonObject T, ReflectedJsonObject U>
struct JsonObjectReflector<ComposedParamsOf<T, U>>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<ComposedParamsOf<T, U>>();
        builder.extend([](auto &object) { return &object.base; });
        builder.extend([](auto &object) { return &object.derived; });
        return builder.build();
    }
};

template<ReflectedJsonObject T>
using CreateParamsOf = ComposedParamsOf<CreateObjectParams, T>;

template<ReflectedJsonObject T>
using GetResultOf = JsonInfo<T>;

template<ReflectedJsonObject T>
GetResultOf<T> getResult(T value)
{
    return {std::move(value)};
}

template<ReflectedJsonObject T>
struct UpdateParamsOf
{
    ObjectId id;
    JsonBuffer<JsonUpdate<T>> settings;
};

template<ReflectedJsonObject T>
struct JsonObjectReflector<UpdateParamsOf<T>>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<UpdateParamsOf<T>>();
        builder.field("id", [](auto &object) { return &object.id; }).description("ID of the object to update");
        builder.field("settings", [](auto &object) { return &object.settings; })
            .description("New object settings (missing fields keep their current value)");
        return builder.build();
    }
};

template<typename T>
T getUpdatedParams(const UpdateParamsOf<T> &params, T current)
{
    auto update = JsonUpdate<T>{std::move(current)};
    params.settings.extract(update);
    return std::move(update.value);
}

using UpdateObjectParams = UpdateParamsOf<ObjectSettings>;
}
