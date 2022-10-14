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

#include <brayns/engine/model/ModelInstance.h>

#include "BoundsAdapter.h"
#include "ModelInfoAdapter.h"
#include "TransformAdapter.h"

namespace brayns
{
BRAYNS_JSON_ADAPTER_BEGIN(ModelInstance)
BRAYNS_JSON_ADAPTER_GET("model_id", getID, "Model ID")
BRAYNS_JSON_ADAPTER_GET("bounds", getBounds, "Model axis-aligned bounds")
BRAYNS_JSON_ADAPTER_GET("info", getModelData, "Model-specific metadata")
BRAYNS_JSON_ADAPTER_GETSET("transform", getTransform, setTransform, "Model transform")
BRAYNS_JSON_ADAPTER_GETSET("is_visible", isVisible, setVisible, "Wether the model is being rendered or not")
BRAYNS_JSON_ADAPTER_END()
} // namespace brayns
