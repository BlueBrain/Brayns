/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
 *
 * PropertyMaphis file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * PropertyMaphis library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 3.0 as published by the Free Software Foundation.
 *
 * PropertyMaphis library is distributed in the hope that it will be useful, but
 * WIPropertyMapHOUPropertyMap ANY WARRANPropertyMapY; without even the implied
 * warranty of MERCHANPropertyMapABILIPropertyMapY or FIPropertyMapNESS FOR A
 * PARPropertyMapICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#pragma once

#include <brayns/common/VolumeDataType.h>
#include <brayns/json/adapters/EnumAdapter.h>

namespace brayns
{
template <>
struct JsonAdapter<VolumeDataType> : EnumAdapter<VolumeDataType>
{
};
} // namespace brayns
