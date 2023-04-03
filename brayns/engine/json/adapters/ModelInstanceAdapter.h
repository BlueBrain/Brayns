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

#include <brayns/engine/model/ModelInstance.h>

#include "BoundsAdapter.h"
#include "ModelInfoAdapter.h"
#include "TransformAdapter.h"

namespace brayns
{
template<>
struct JsonAdapter<ModelInstance> : ObjectAdapter<ModelInstance>
{
    static void reflect()
    {
        title("ModelInstance");
        get("model_id", [](auto &object) { return object.getID(); }).description("Model ID");
        get(
            "model_type",
            [](auto &object) -> auto & { return object.getModelType(); })
            .description("Model type");
        get(
            "bounds",
            [](auto &object) -> auto & { return object.getBounds(); })
            .description("Model bounds");
        get("info", [](auto &object) { return object.getModelData(); }).description("Model-specific info");
        getset(
            "transform",
            [](auto &object) -> auto & { return object.getTransform(); },
            [](auto &object, const auto &value) { object.setTransform(value); })
            .description("Model transform")
            .required(false);
        getset(
            "is_visible",
            [](auto &object) { return object.isVisible(); },
            [](auto &object, auto value) { object.setVisible(value); })
            .description("Wether the model is being rendered or not")
            .required(false);
    }
};
} // namespace brayns
