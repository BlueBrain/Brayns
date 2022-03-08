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
#include <brayns/json/JsonObjectMacro.h>

#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>

#include "BoundsAdapter.h"
#include "TransformationAdapter.h"

namespace brayns
{
class ReadModelProxy
{
public:
    ReadModelProxy() = default;
    ReadModelProxy(const ModelInstance &mi);

    uint32_t getId() const noexcept;
    const Bounds &getBounds() const noexcept;
    const Model::Metadata &getMetadata() const noexcept;
    const Transformation &getTransform() const noexcept;
    bool getVisible() const noexcept;

private:
    const ModelInstance *_instance{nullptr};
};

BRAYNS_JSON_ADAPTER_BEGIN(ReadModelProxy)
BRAYNS_JSON_ADAPTER_GET("model_id", getId, "Model ID")
BRAYNS_JSON_ADAPTER_GET("bounds", getBounds, "Model axis-aligned bounds")
BRAYNS_JSON_ADAPTER_GET("metadata", getMetadata, "Model-specific metadata")
BRAYNS_JSON_ADAPTER_GET("transformation", getTransform, "Model transformation")
BRAYNS_JSON_ADAPTER_GET("is_visible", getVisible, "Wether the model is being rendered or not")
BRAYNS_JSON_ADAPTER_END()

class UpdateModelProxy
{
public:
    UpdateModelProxy() = default;
    UpdateModelProxy(ModelInstance &instance);

    const Transformation &getTransform() const noexcept;
    void setTransform(const Transformation &transformation) noexcept;
    bool getVisible() const noexcept;
    void setVisible(bool visible) noexcept;

private:
    ModelInstance *_modelInstance{nullptr};
};

BRAYNS_JSON_ADAPTER_BEGIN(UpdateModelProxy)
BRAYNS_JSON_ADAPTER_GETSET("transformation", getTransform, setTransform, "Model transformation", Required(false))
BRAYNS_JSON_ADAPTER_GETSET("visible", getVisible, setVisible, "Model visibility", Required(false))
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_OBJECT_BEGIN(UpdateModelParams)
BRAYNS_JSON_OBJECT_ENTRY(uint32_t, model_id, "Model ID")
BRAYNS_JSON_OBJECT_ENTRY(UpdateModelProxy, parameters, "Parameters to update in the model")
BRAYNS_JSON_OBJECT_END()
} // namespace brayns
