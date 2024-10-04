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

#include "ServiceEndpoints.h"

#include <brayns/core/Version.h>

namespace brayns
{
struct VersionResult
{
    int major = BRAYNS_VERSION_MAJOR;
    int minor = BRAYNS_VERSION_MINOR;
    int patch = BRAYNS_VERSION_PATCH;
    int preRelease = BRAYNS_VERSION_PRE_RELEASE;
    std::string tag = getVersionTag();
};

template<>
struct JsonObjectReflector<VersionResult>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<VersionResult>();
        builder.field("major", [](auto &object) { return &object.major; }).description("Major version");
        builder.field("minor", [](auto &object) { return &object.minor; }).description("Minor version");
        builder.field("patch", [](auto &object) { return &object.patch; }).description("Patch version");
        builder.field("preRelease", [](auto &object) { return &object.preRelease; }).description("Pre-release version (0 if stable)");
        builder.field("tag", [](auto &object) { return &object.tag; }).description("Version tag major.minor.patch[-prerelease]");
        return builder.build();
    }
};

struct SchemaParams
{
    std::string method;
};

template<>
struct JsonObjectReflector<SchemaParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<SchemaParams>();
        builder.field("method", [](auto &object) { return &object.method; }).description("Method of the endpoint whose schema is requested");
        return builder.build();
    }
};

void addServiceEndpoints(ApiBuilder &builder, Api &api, StopToken &token)
{
    builder.endpoint("getVersion", [] { return VersionResult(); }).description("Get the build version of the service currently running");

    builder.endpoint("getMethods", [&] { return api.getMethods(); }).description("Get available JSON-RPC methods");

    builder.endpoint("getSchema", [&](SchemaParams params) { return api.getSchema(params.method); })
        .description("Get the schema of the given JSON-RPC method");

    builder.endpoint("stop", [&] { token.stop(); }).description("Cancel all running tasks, close all connections and stop the service");
}
}
