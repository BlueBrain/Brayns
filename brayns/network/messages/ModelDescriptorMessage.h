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

#include <brayns/network/message/Message.h>

#include <brayns/engine/Model.h>

#include "BoxAdapter.h"
#include "TransformationMessage.h"

namespace brayns
{
BRAYNS_MESSAGE_BEGIN(ModelDescriptorMessage)
BRAYNS_MESSAGE_ENTRY(bool, bounding_box, "Has bounding box")
BRAYNS_MESSAGE_ENTRY(Boxd, bounds, "Model bounds")
BRAYNS_MESSAGE_ENTRY(size_t, id, "Model ID")
BRAYNS_MESSAGE_ENTRY(ModelMetadata, metadata, "Key-value metadata")
BRAYNS_MESSAGE_ENTRY(std::string, name, "Model name")
BRAYNS_MESSAGE_ENTRY(std::string, path, "Model file path")
BRAYNS_MESSAGE_ENTRY(TransformationMessage, transformation, "Transformation")
BRAYNS_MESSAGE_ENTRY(bool, visible, "Is visible")
BRAYNS_MESSAGE_ENTRY(std::string, loader_name, "Name of the loader")
BRAYNS_MESSAGE_ENTRY(PropertyMap, loader_properties, "Loader properties")
BRAYNS_MESSAGE_END()
} // namespace brayns