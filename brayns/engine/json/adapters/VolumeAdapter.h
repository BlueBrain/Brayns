/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/engine/volume/VolumeDataType.h>
#include <brayns/engine/volume/types/RegularVolume.h>
#include <brayns/json/JsonAdapterMacro.h>

namespace brayns
{
template<>
struct JsonAdapter<VolumeDataType> : EnumAdapter<VolumeDataType>
{
};

BRAYNS_JSON_ADAPTER_BEGIN(RegularVolume)
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("data_type", dataType, "Voxel data type")
BRAYNS_JSON_ADAPTER_ENTRY(voxels, "Voxel data as byte array")
BRAYNS_JSON_ADAPTER_ENTRY(size, "Width, height and depth of the volume")
BRAYNS_JSON_ADAPTER_ENTRY(spacing, "Voxel dimensions in world space")
BRAYNS_JSON_ADAPTER_NAMED_ENTRY(
    "data_on_vertex",
    perVertexData,
    "Specifies if the voxel is located at the cell vertices or in the center")
BRAYNS_JSON_ADAPTER_END()
}
