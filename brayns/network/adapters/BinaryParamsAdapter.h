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

#include <brayns/tasks/AddModelFromBlobTask.h>

#include "BoxAdapter.h"
#include "TransformationAdapter.h"

namespace brayns
{
BRAYNS_ADAPTER_BEGIN(BinaryParam)
BRAYNS_ADAPTER_GETSET("bounding_box", getBoundingBox, setBoundingBox,
                      "Bounding box");
BRAYNS_ADAPTER_NAMED_ENTRY("chunks_id", chunksID, "Chunk ID");
BRAYNS_ADAPTER_GETSET("name", getName, setName, "Model name");
BRAYNS_ADAPTER_GETSET("path", getPath, setPath, "Model source path");
BRAYNS_ADAPTER_NAMED_ENTRY("size", size, "File size in bytes");
BRAYNS_ADAPTER_GETSET("transformation", getTransformation, setTransformation,
                      "Model transformation");
BRAYNS_ADAPTER_NAMED_ENTRY("type", type, "File extension or type");
BRAYNS_ADAPTER_GETSET("visible", getVisible, setVisible, "Visible");
BRAYNS_ADAPTER_GETSET("loader_name", getLoaderName, setLoaderName,
                      "Loader name");
BRAYNS_ADAPTER_GETSET("loader_properties", getLoaderProperties,
                      setLoaderProperties, "Loader properties");
BRAYNS_ADAPTER_END()
} // namespace brayns