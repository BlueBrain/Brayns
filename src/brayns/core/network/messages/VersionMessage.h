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
struct VersionMessage
{
    int major = 0;
    int minor = 0;
    int patch = 0;
    int pre_release = 0;
    std::string tag;
    std::string copyright;
};

template<>
struct JsonAdapter<VersionMessage> : ObjectAdapter<VersionMessage>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("VersionMessage");
        builder.get("major", [](auto &object) { return object.major; }).description("Major version");
        builder.get("minor", [](auto &object) { return object.minor; }).description("Minor version");
        builder.get("patch", [](auto &object) { return object.patch; }).description("Patch version");
        builder.get("pre_release", [](auto &object) { return object.pre_release; }).description("Pre-release");
        builder
            .get(
                "tag",
                [](auto &object) -> auto & { return object.tag; })
            .description("Full tag major.minor.path[-preRelease]");
        builder
            .get(
                "copyright",
                [](auto &object) -> auto & { return object.copyright; })
            .description("Copyright statement");
        return builder.build();
    }
};
} // namespace brayns
