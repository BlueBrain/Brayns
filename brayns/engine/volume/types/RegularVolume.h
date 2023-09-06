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

#include <brayns/engine/volume/Volume.h>
#include <brayns/engine/volume/VolumeDataType.h>
#include <brayns/utils/MathTypes.h>

#include <vector>

namespace brayns
{
/**
 * @brief The RegularVolume struct is a regular grid volume in which the data is laid out in XYZ order:
 * The first elements are the X values of the first row of the first frame.
 */
struct RegularVolume
{
    VolumeDataType dataType;
    std::vector<uint8_t> voxels;
    Vector3ui size;
    Vector3f spacing = Vector3f(1.f);
    bool perVertexData = false;
};

template<>
class VolumeTraits<RegularVolume>
{
public:
    static inline const std::string handleName = "structuredRegular";
    static inline const std::string name = "gridvolume";

    static Bounds computeBounds(const TransformMatrix &matrix, const RegularVolume &data);

    static void updateData(ospray::cpp::Volume &handle, RegularVolume &volumeData);
};
}
