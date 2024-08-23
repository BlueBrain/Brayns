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

struct ObjectInfo
{
    ObjectId id;
    std::string type;
    JsonValue userData = {};
};

template<>
struct JsonObjectReflector<ObjectInfo>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<ObjectInfo>();
        builder.field("id", [](auto &object) { return &object.id; })
            .description("Object ID (starts at 1, uses 0 for objects that are not in registry)");
        builder.field("type", [](auto &object) { return &object.type; })
            .description("Object type, use 'get{type}' to query detailed information about the object");
        builder.field("userData", [](auto &object) { return &object.userData; })
            .description("Data set by user (not used by Brayns)");
        return builder.build();
    }
};

struct ObjectParams
{
    ObjectId id;
};

using ObjectResult = ObjectParams;

template<>
struct JsonObjectReflector<ObjectParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<ObjectParams>();
        builder.field("id", [](auto &object) { return &object.id; }).description("Object ID");
        return builder.build();
    }
};

template<ReflectedJson T>
struct UpdateParams
{
    ObjectId id;
    T properties;
};

template<ReflectedJson T>
struct JsonObjectReflector<UpdateParams<T>>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<UpdateParams<T>>();
        builder.field("id", [](auto &object) { return &object.id; }).description("ID of the object to update");
        builder.field("properties", [](auto &object) { return &object.properties; })
            .description("New object properties");
        return builder.build();
    }
};

template<ReflectedJson Base, ReflectedJson Derived>
struct ComposedParams
{
    Base base;
    Derived derived;
};

template<ReflectedJson Base, ReflectedJson Derived>
struct JsonObjectReflector<ComposedParams<Base, Derived>>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<ComposedParams<Base, Derived>>();

        if (!std::is_same_v<Base, NullJson>)
        {
            builder.field("base", [](auto &object) { return &object.base; })
                .description("Base properties common to all derived objects (camera, renderer)");
        }

        builder.field("derived", [](auto &object) { return &object.derived; })
            .description("Derived properties that are specific to the object type (perspective, scivis)");

        return builder.build();
    }
};
}
