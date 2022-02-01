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

#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>

#include <brayns/json/JsonAdapterMacro.h>

#include "TransformationAdapter.h"

namespace brayns
{
class UpdateModelProxy
{
public:
    UpdateModelProxy() = default;
    UpdateModelProxy(Scene &scene);

    void setId(size_t id);
    void setBoundingBox(bool enabled);
    void setName(const std::string &name);
    void setTransformation(const Transformation &transformation);
    void setVisible(bool visible);
    void computeBounds();

private:
    Scene *_scene = nullptr;
    ModelDescriptor *_model = nullptr;
};

BRAYNS_NAMED_JSON_ADAPTER_BEGIN(UpdateModelProxy, "UpdateModelParams")
BRAYNS_JSON_ADAPTER_SET("id", setId, "Model ID", Required())
BRAYNS_JSON_ADAPTER_SET("bounding_box", setBoundingBox, "Display model bounds")
BRAYNS_JSON_ADAPTER_SET("name", setName, "Model name")
BRAYNS_JSON_ADAPTER_SET("transformation", setTransformation, "Model transformation")
BRAYNS_JSON_ADAPTER_SET("visible", setVisible, "Model visibility")
BRAYNS_JSON_ADAPTER_END()
} // namespace brayns
