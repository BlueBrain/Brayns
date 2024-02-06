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

#include <brayns/engine/json/adapters/ModelInstanceAdapter.h>

namespace brayns
{
struct UpdateModelMessage
{
    uint32_t model_id = 0;
    JsonBuffer<ModelInstance> model;
};

template<>
struct JsonAdapter<UpdateModelMessage> : ObjectAdapter<UpdateModelMessage>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("UpdateModelMessage");
        builder
            .getset(
                "model_id",
                [](auto &object) { return object.model_id; },
                [](auto &object, auto value) { object.model_id = value; })
            .description("Model ID");
        builder
            .getset(
                "model",
                [](auto &object) -> auto & { return object.model; },
                [](auto &object, const auto &value) { object.model = value; })
            .description("Model data to update");
        return builder.build();
    }
};
} // namespace brayns
