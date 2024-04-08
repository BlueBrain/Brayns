/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 *
 * Responsible Author: Daniel.Nachbaur@epfl.ch
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
#include <vector>

#include <brayns/core/json/Json.h>

namespace brayns
{
struct LoaderInfo
{
    std::string plugin;
    std::string name;
    std::vector<std::string> extensions;
    bool binary = false;
    JsonSchema schema;
};

template<>
struct JsonAdapter<LoaderInfo> : ObjectAdapter<LoaderInfo>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("Loader");
        builder
            .get(
                "plugin",
                [](auto &object) -> auto & { return object.plugin; })
            .description("Plugin required to use the loader");
        builder
            .get(
                "name",
                [](auto &object) -> auto & { return object.name; })
            .description("Loader name");
        builder
            .get(
                "extensions",
                [](auto &object) -> auto & { return object.extensions; })
            .description("Supported file formats / extensions");
        builder.get("binary", [](auto &object) { return object.binary; })
            .description("True if loader supports loading binary data");
        builder
            .get(
                "input_parameters_schema",
                [](auto &object) -> auto & { return object.schema; })
            .description("Loader params schema");
        return builder.build();
    }
};
} // namespace brayns
