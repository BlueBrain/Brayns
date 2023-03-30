/* Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

namespace brayns
{
struct AddModelMessage
{
    std::string path;
    std::string loader_name;
    JsonValue loader_properties;
};

template<>
struct JsonAdapter<AddModelMessage> : ObjectAdapter<AddModelMessage>
{
    static void reflect()
    {
        title("AddModelMessage");
        set<std::string>("path", [](auto &object, auto value) { object.path = std::move(value); })
            .description("Path of the file to load");
        set<std::string>("loader_name", [](auto &object, auto value) { object.loader_name = std::move(value); })
            .description("Name of the loader used to parse the model file");
        set<JsonValue>("loader_properties", [](auto &object, const auto &value) { object.loader_properties = value; })
            .description("Settings to configure the loading process");
    }
};
} // namespace brayns
