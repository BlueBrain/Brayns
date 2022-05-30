/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <api/DataMangler.h>
#include <brayns/common/MathTypes.h>

class AtlasVolume
{
public:
    AtlasVolume(
        const brayns::Vector3ui &size,
        const brayns::Vector3f &spacing,
        size_t voxelSize,
        std::unique_ptr<IDataMangler> data);

    const brayns::Vector3ui &getSize() const noexcept;

    const brayns::Vector3f &getSpacing() const noexcept;

    size_t getVoxelSize() const noexcept;

    const IDataMangler &getData() const noexcept;

private:
    brayns::Vector3ui _size;
    brayns::Vector3f _spacing;
    size_t _voxelSize;
    std::unique_ptr<IDataMangler> _data;
};
