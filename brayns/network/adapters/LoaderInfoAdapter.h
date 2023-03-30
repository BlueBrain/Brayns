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

#include <brayns/io/LoaderRegistry.h>

#include <brayns/json/Json.h>

namespace brayns
{
template<>
struct JsonAdapter<LoaderInfo> : ObjectAdapter<LoaderInfo>
{
    static void reflect()
    {
        title("LoaderInfo");
        get("name", [](auto &object) -> decltype(auto) { return object.name; }).description("Loader name");
        get("extensions", [](auto &object) -> decltype(auto) { return object.extensions; })
            .description("Supported file extensions");
        get("input_parameters_schema", [](auto &object) -> decltype(auto) { return object.inputParametersSchema; })
            .description("Loader properties");
    }
};
} // namespace brayns
