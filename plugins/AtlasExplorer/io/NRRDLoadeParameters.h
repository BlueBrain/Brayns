/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include <brayns/json/JsonAdapterMacro.h>
#include <brayns/json/JsonObjectMacro.h>

#include <api/VoxelType.h>

namespace brayns
{
BRAYNS_JSON_ADAPTER_ENUM(
    VoxelType,
    {"scalar", VoxelType::Scalar},
    {"orientation", VoxelType::Orientation},
    {"flatmap", VoxelType::Flatmap},
    {"layer_distance", VoxelType::LayerDistance})
}

BRAYNS_JSON_OBJECT_BEGIN(NRRDLoaderParameters)
BRAYNS_JSON_OBJECT_ENTRY(VoxelType, type, "Voxel type to interpret the atlas being loaded")
BRAYNS_JSON_OBJECT_END()
