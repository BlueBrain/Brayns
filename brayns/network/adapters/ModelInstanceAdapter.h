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

    uint32_t getId() const;
    const Bounds &getBounds() const;
    const Model::Metadata &getMetadata() const;
    const Transformation &getTransform() const;
    bool getIsVisible() const;

private:
    const ModelInstance &getModel() const;

private:
    const ModelInstance *_instance{nullptr};
};

BRAYNS_JSON_ADAPTER_BEGIN(ReadModelProxy)
BRAYNS_JSON_ADAPTER_GET("model_id", getId, "Model ID")
BRAYNS_JSON_ADAPTER_GET("bounding_box", getBounds, "Model instance AABB")
BRAYNS_JSON_ADAPTER_GET("metadata", getMetadata, "Model metadata")
BRAYNS_JSON_ADAPTER_GET("transformation", getTransform, "Model transformation")
BRAYNS_JSON_ADAPTER_GET("visible", getIsVisible, "Wether the model is rendered or not")
BRAYNS_JSON_ADAPTER_END()

class UpdateModelProxy
{
public:
    UpdateModelProxy(Scene &scene);

    void setId(uint32_t id);
    void setTransformation(const Transformation &transformation);
    void setVisible(bool visible);

private:
    Scene &_scene;
    ModelInstance *_modelInstance{nullptr};
};

BRAYNS_NAMED_JSON_ADAPTER_BEGIN(UpdateModelProxy, "UpdateModelParams")
BRAYNS_JSON_ADAPTER_SET("id", setId, "Model ID", Required())
BRAYNS_JSON_ADAPTER_SET("transformation", setTransformation, "Model transformation")
BRAYNS_JSON_ADAPTER_SET("visible", setVisible, "Model visibility")
BRAYNS_JSON_ADAPTER_END()
} // namespace brayns
