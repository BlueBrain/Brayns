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

#include "LoadInfoAdapter.h"
#include "TransformAdapter.h"

#include <brayns/engine/model/ModelInfo.h>

namespace brayns
{
template<>
struct JsonAdapter<ModelInfo> : ObjectAdapter<ModelInfo>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("ModelInfo");
        builder.get("load_info", [](auto &object) { return object.getLoadInfo(); })
            .description("Model load info")
            .required(false);
        builder.get("metadata", [](auto &object) { return object.getMetadata(); })
            .description("Model-specific metadata")
            .required(false);
        builder.get("base_transform", [](auto &object) { return object.getBaseTransform(); })
            .description("Model transform")
            .required(false);
        return builder.build();
    }
};
} // namespace brayns
