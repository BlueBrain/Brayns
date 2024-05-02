/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <brayns/core/json/Json.h>

#include <brayns/atlas/api/VoxelType.h>

struct NRRDLoaderParameters
{
    VoxelType type = VoxelType::Scalar;
};

namespace brayns
{
template<>
struct EnumReflector<VoxelType>
{
    static EnumMap<VoxelType> reflect()
    {
        return {
            {"scalar", VoxelType::Scalar},
            {"orientation", VoxelType::Orientation},
            {"flatmap", VoxelType::Flatmap},
            {"layer_distance", VoxelType::LayerDistance}};
    }
};

template<>
struct JsonAdapter<VoxelType> : EnumAdapter<VoxelType>
{
};

template<>
struct JsonAdapter<NRRDLoaderParameters> : ObjectAdapter<NRRDLoaderParameters>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("NRRDLoaderParameters");
        builder
            .getset(
                "type",
                [](auto &object) { return object.type; },
                [](auto &object, auto value) { object.type = value; })
            .description("Voxel type to interpret the atlas being loaded");
        return builder.build();
    }
};
} // namespace brayns