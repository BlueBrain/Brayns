/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/utils/EnumUtils.h>

namespace brayns
{
enum class VolumeDataType
{
    FLOAT,
    DOUBLE,
    UINT8,
    UINT16,
    UINT32,
    INT8,
    INT16,
    INT32
};

template <>
inline std::vector<std::pair<std::string, VolumeDataType>> enumMap()
{
    return {
        {"float", VolumeDataType::FLOAT},   {"double", VolumeDataType::DOUBLE},
        {"uint8", VolumeDataType::UINT8},   {"uint16", VolumeDataType::UINT16},
        {"uint32", VolumeDataType::UINT32}, {"int8", VolumeDataType::INT8},
        {"int16", VolumeDataType::INT16},   {"int32", VolumeDataType::INT32}};
}
} // namespace brayns
