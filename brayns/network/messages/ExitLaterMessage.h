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

#include <brayns/json/Json.h>

namespace brayns
{
struct ExitLaterMessage
{
    uint32_t minutes = 0;
};

template<>
struct JsonAdapter<ExitLaterMessage> : ObjectAdapter<ExitLaterMessage>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("ExitLaterMessage");
        builder
            .getset(
                "minutes",
                [](auto &object) { return object.minutes; },
                [](auto &object, auto value) { object.minutes = value; })
            .description("Number of minutes after which Brayns will shut down");
        return builder.build();
    }
};
} // namespace brayns
