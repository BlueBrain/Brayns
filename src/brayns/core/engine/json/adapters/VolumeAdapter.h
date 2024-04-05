/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include <brayns/core/engine/volume/VolumeDataType.h>
#include <brayns/core/engine/volume/types/RegularVolume.h>

#include <brayns/core/json/Json.h>

namespace brayns
{
template<>
struct JsonAdapter<VolumeDataType> : EnumAdapter<VolumeDataType>
{
};

template<>
struct JsonAdapter<RegularVolume> : ObjectAdapter<RegularVolume>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("RegularVolume");
        builder
            .getset(
                "data_type",
                [](auto &object) { return object.dataType; },
                [](auto &object, auto value) { object.dataType = value; })
            .description("Voxel data as byte array");
        builder
            .getset(
                "voxels",
                [](auto &object) -> auto & { return object.voxels; },
                [](auto &object, auto value) { object.voxels = std::move(value); })
            .description("Voxel data type");
        builder
            .getset(
                "size",
                [](auto &object) -> auto & { return object.size; },
                [](auto &object, const auto &value) { object.size = value; })
            .description("Width, height and depth of the volume");
        builder
            .getset(
                "spacing",
                [](auto &object) -> auto & { return object.spacing; },
                [](auto &object, const auto &value) { object.spacing = value; })
            .description("Voxel dimensions in world space");
        builder
            .getset(
                "data_on_vertex",
                [](auto &object) { return object.perVertexData; },
                [](auto &object, auto value) { object.perVertexData = value; })
            .description("Specify if the voxel is located at the cell vertices or in the center");
        return builder.build();
    }
};
} // namespace brayns
