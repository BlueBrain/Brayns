/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/json/Json.h>

#include <brayns/engine/json/EngineObjectData.h>

namespace brayns
{
template<>
struct JsonAdapter<EngineObjectData> : ObjectAdapter<EngineObjectData>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("EngineObjectData");
        builder
            .getset(
                "name",
                [](auto &object) -> auto & { return object.name; },
                [](auto &object, auto value) { object.name = std::move(value); })
            .description("Object type name")
            .required(false);
        builder
            .getset(
                "params",
                [](auto &object) -> auto & { return object.params; },
                [](auto &object, const auto &value) { object.params = value; })
            .description("Object parameters")
            .required(false);
        return builder.build();
    }
};
} // namespace brayns
