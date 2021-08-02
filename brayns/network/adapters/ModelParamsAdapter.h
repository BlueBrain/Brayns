/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#include <brayns/network/json/MessageAdapter.h>

#include <brayns/engine/Model.h>

#include "BoxAdapter.h"
#include "TransformationAdapter.h"

namespace brayns
{
BRAYNS_ADAPTER_BEGIN(ModelParams)
BRAYNS_ADAPTER_GETSET(bool, "bounding_box", getBoundingBox, setBoundingBox,
                      "Display bounding box")
BRAYNS_ADAPTER_GETSET(std::string, "name", getName, setName, "Model name")
BRAYNS_ADAPTER_GETSET(std::string, "path", getPath, setPath, "Model file path")
BRAYNS_ADAPTER_GETSET(Transformation, "transformation", getTransformation,
                      setTransformation, "Model transformation")
BRAYNS_ADAPTER_GETSET(bool, "visible", getVisible, setVisible, "Is visible")
BRAYNS_ADAPTER_GETSET(std::string, "loader_name", getLoaderName, setLoaderName,
                      "Name of the loader")
BRAYNS_ADAPTER_GETSET(PropertyMap, "loader_properties", getLoaderProperties,
                      setLoaderProperties, "Loader properties")
BRAYNS_ADAPTER_END()
} // namespace brayns