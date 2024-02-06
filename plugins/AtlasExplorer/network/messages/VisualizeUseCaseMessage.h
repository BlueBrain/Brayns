/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

struct VisualizeUseCaseMessage
{
    uint32_t model_id = 0;
    std::string use_case;
    brayns::JsonValue params;
};

namespace brayns
{
template<>
struct JsonAdapter<VisualizeUseCaseMessage> : ObjectAdapter<VisualizeUseCaseMessage>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("VisualizeUseCaseMessage");
        builder
            .getset(
                "model_id",
                [](auto &object) { return object.model_id; },
                [](auto &object, auto value) { object.model_id = value; })
            .description("ID of the model holding an atlas volume");
        builder
            .getset(
                "use_case",
                [](auto &object) -> auto & { return object.use_case; },
                [](auto &object, auto value) { object.use_case = std::move(value); })
            .description("Use case name");
        builder
            .getset(
                "params",
                [](auto &object) -> auto & { return object.params; },
                [](auto &object, const auto &value) { object.params = value; })
            .description("Additional use case parameters");
        return builder.build();
    }
};
} // namespace brayns
