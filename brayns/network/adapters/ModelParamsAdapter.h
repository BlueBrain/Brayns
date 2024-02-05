/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include "BoxAdapter.h"
#include "TransformationAdapter.h"

namespace brayns
{
BRAYNS_JSON_ADAPTER_BEGIN(ModelParams)
BRAYNS_JSON_ADAPTER_GETSET("bounding_box", getBoundingBox, setBoundingBox,
                           "Display bounding box")
BRAYNS_JSON_ADAPTER_GETSET("name", getName, setName, "Model name")
BRAYNS_JSON_ADAPTER_GETSET("path", getPath, setPath, "Model file path")
BRAYNS_JSON_ADAPTER_GETSET("transformation", getTransformation,
                           setTransformation, "Model transformation")
BRAYNS_JSON_ADAPTER_GETSET("visible", getVisible, setVisible, "Is visible")
BRAYNS_JSON_ADAPTER_GETSET("loader_name", getLoaderName, setLoaderName,
                           "Name of the loader")
BRAYNS_JSON_ADAPTER_GETSET("loader_properties", getLoadParameters,
                           setLoadParameters, "Loader properties")
BRAYNS_JSON_ADAPTER_END()
} // namespace brayns
