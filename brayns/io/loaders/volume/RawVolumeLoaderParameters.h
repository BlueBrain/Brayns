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

#include <brayns/engine/json/adapters/VolumeAdapter.h>

#include <brayns/json/Json.h>

namespace brayns
{
struct RawVolumeLoaderParameters
{
    Vector3ui dimensions{0};
    Vector3f spacing{0};
    VolumeDataType data_type = VolumeDataType::UnsignedChar;
};

template<>
struct JsonAdapter<RawVolumeLoaderParameters> : ObjectAdapter<RawVolumeLoaderParameters>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("RawVolumeLoaderParameters");
        builder
            .getset(
                "dimensions",
                [](auto &object) -> auto & { return object.dimensions; },
                [](auto &object, const auto &value) { object.dimensions = value; })
            .description("Volume grid size XYZ");
        builder
            .getset(
                "spacing",
                [](auto &object) -> auto & { return object.spacing; },
                [](auto &object, const auto &value) { object.spacing = value; })
            .description("Volume grid cell spacing XYZ");
        builder
            .getset(
                "data_type",
                [](auto &object) { return object.data_type; },
                [](auto &object, auto value) { object.data_type = value; })
            .description("Volume byte data type");
        return builder.build();
    }
};
} // namespace brayns
