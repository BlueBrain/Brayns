/* Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

#include <brayns/json/Json.h>

namespace brayns
{
struct VersionMessage
{
    int major = 0;
    int minor = 0;
    int patch = 0;
    std::string revision;
};

template<>
struct JsonAdapter<VersionMessage> : ObjectAdapter<VersionMessage>
{
    static void reflect()
    {
        title("VersionMessage");
        get("major", [](auto &object) { return object.major; }).description("Major version");
        get("minor", [](auto &object) { return object.minor; }).description("Minor version");
        get("patch", [](auto &object) { return object.patch; }).description("Patch version");
        get("revision", [](auto &object) -> decltype(auto) { return object.revision; }).description("SCM revision");
    }
};
} // namespace brayns
