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

#include <brayns/engine/json/adapters/TransformAdapter.h>

namespace brayns
{
struct InstantiateMessage
{
    uint32_t model_id = 0;
    std::vector<Transform> transforms;
};

template<>
struct JsonAdapter<InstantiateMessage> : ObjectAdapter<InstantiateMessage>
{
    static void reflect()
    {
        title("InstantiateMessage");
        getset(
            "model_id",
            [](auto &object) { return object.model_id; },
            [](auto &object, auto value) { object.model_id = value; })
            .description("ID of the model to instantiate");
        getset(
            "transforms",
            [](auto &object) -> auto & { return object.transforms; },
            [](auto &object, auto value) { object.transforms = std::move(value); })
            .description("Transformations to apply to the new instances");
    }
};
} // namespace brayns
