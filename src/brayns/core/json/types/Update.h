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

#include "Objects.h"

namespace brayns
{
template<ReflectedJsonObject T>
struct JsonUpdate
{
    T value;
};

template<ReflectedJsonObject T>
struct JsonObjectReflector<JsonUpdate<T>>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<JsonUpdate<T>>();
        builder.extend([](auto &object) { return &object.value; });

        auto info = builder.build();

        for (auto &field : info.fields)
        {
            field.schema.required = false;
            field.schema.defaultValue.reset();
        }

        return info;
    }
};
}
