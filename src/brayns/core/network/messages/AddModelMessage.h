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

#include <brayns/core/json/Json.h>

namespace brayns
{
struct AddModelParams
{
    std::string path;
    std::string loader_name;
    JsonValue loader_properties;
};

template<>
struct JsonAdapter<AddModelParams> : ObjectAdapter<AddModelParams>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("AddModelParams");
        builder
            .getset(
                "path",
                [](auto &object) -> auto & { return object.path; },
                [](auto &object, auto value) { object.path = std::move(value); })
            .description("Path of the file to load");
        builder
            .getset(
                "loader_name",
                [](auto &object) -> auto & { return object.loader_name; },
                [](auto &object, auto value) { object.loader_name = std::move(value); })
            .description("Name of the loader used to parse the model file");
        builder
            .getset(
                "loader_properties",
                [](auto &object) -> auto & { return object.loader_properties; },
                [](auto &object, const auto &value) { object.loader_properties = value; })
            .description("Settings to configure the loading process");
        return builder.build();
    }
};
} // namespace brayns
