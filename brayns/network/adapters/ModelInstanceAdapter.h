/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include <brayns/json/JsonAdapterMacro.h>
#include <brayns/json/JsonBuffer.h>
#include <brayns/json/JsonObjectMacro.h>

#include <brayns/engine/ModelInstance.h>

#include <brayns/network/messages/GetModelMessage.h>

#include "BoundsAdapter.h"
#include "TransformAdapter.h"

namespace brayns
{
class ModelInstanceProxy
{
public:
    ModelInstanceProxy() = default;
    ModelInstanceProxy(ModelInstance &modelInstance);

    uint32_t getID() const noexcept;

    Bounds getBounds() const noexcept;

    std::map<std::string, std::string> getModelMetadata() const noexcept;

    Transform getTransform() const noexcept;

    void setTransform(const Transform &transform) noexcept;

    bool isVisible() const noexcept;

    void setVisible(const bool val) noexcept;

private:
    ModelInstance *_modelInstance = nullptr;
};

BRAYNS_JSON_ADAPTER_BEGIN(ModelInstanceProxy)
BRAYNS_JSON_ADAPTER_GET("model_id", getID, "Model ID")
BRAYNS_JSON_ADAPTER_GET("bounds", getBounds, "Model axis-aligned bounds")
BRAYNS_JSON_ADAPTER_GET("metadata", getModelMetadata, "Model-specific metadata")
BRAYNS_JSON_ADAPTER_GETSET("transform", getTransform, setTransform, "Model transform")
BRAYNS_JSON_ADAPTER_GETSET("is_visible", isVisible, setVisible, "Wether the model is being rendered or not")
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_OBJECT_BEGIN(UpdateModelMessage)
BRAYNS_JSON_OBJECT_ENTRY(size_t, model_id, "Model ID")
BRAYNS_JSON_OBJECT_ENTRY(JsonBuffer<ModelInstanceProxy>, model, "Model data to update")
BRAYNS_JSON_OBJECT_END()
} // namespace brayns
