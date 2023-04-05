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
struct InspectMessage
{
    Vector2f position{0.0f};
};

template<>
struct JsonAdapter<InspectMessage> : ObjectAdapter<InspectMessage>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("InspectMessage");
        builder
            .getset(
                "position",
                [](auto &object) -> auto & { return object.position; },
                [](auto &object, const auto &value) { object.position = value; })
            .description("Normalized screen position XY");
        return builder.build();
    }
};

struct InspectResult
{
    bool hit = false;
    Vector3f position{0.0f};
    uint32_t model_id = 0;
    JsonValue metadata;
};

template<>
struct JsonAdapter<InspectResult> : ObjectAdapter<InspectResult>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("InspectResult");
        builder.get("hit", [](auto &object) { return object.hit; })
            .description("True if a model was at given position, otherwise the rest is invalid");
        builder
            .get(
                "position",
                [](auto &object) -> auto & { return object.position; })
            .description("World position XYZ where the model was hit");
        builder.get("model_id", [](auto &object) { return object.model_id; })
            .description("ID of the model that was hit at given position");
        builder
            .get(
                "metadata",
                [](auto &object) -> auto & { return object.metadata; })
            .description("Extra attributes depending on the type of model hitted");
        return builder.build();
    }
};
} // namespace brayns
