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

#include <brayns/core/json/Json.h>

namespace brayns
{
struct SchemaParams
{
    std::string endpoint;
};

template<>
struct JsonAdapter<SchemaParams> : ObjectAdapter<SchemaParams>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("SchemaParams");
        builder
            .getset(
                "endpoint",
                [](auto &object) -> auto & { return object.endpoint; },
                [](auto &object, auto value) { object.endpoint = std::move(value); })
            .description("Name of the endpoint");
        return builder.build();
    }
};

struct SchemaResult
{
    std::string plugin;
    std::string title;
    std::string description;
    bool async = false;
    bool deprecated = false;
    std::optional<JsonSchema> params;
    std::optional<JsonSchema> returns;
};

template<>
struct JsonAdapter<SchemaResult> : ObjectAdapter<SchemaResult>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("SchemaResult");
        builder
            .get(
                "plugin",
                [](auto &object) -> auto & { return object.plugin; })
            .description("Name of the plugin that loads the entrypoint");
        builder
            .get(
                "title",
                [](auto &object) -> auto & { return object.title; })
            .description("Name of the entrypoint (method)");
        builder
            .get(
                "description",
                [](auto &object) -> auto & { return object.description; })
            .description("Description of the entrypoint");
        builder.get("async", [](auto &object) { return object.async; })
            .description("Check if the entrypoint is asynchronous (send progress and can be cancelled)");
        builder.get("deprecated", [](auto &object) { return object.deprecated; })
            .description("If true, the entrypoint will be removed / renamed in the next release");
        builder
            .get(
                "params",
                [](auto &object) -> auto & { return object.params; })
            .description("Input schema")
            .required(false);
        builder
            .get(
                "returns",
                [](auto &object) -> auto & { return object.returns; })
            .description("Output schema")
            .required(false);
        return builder.build();
    }
};
} // namespace brayns
