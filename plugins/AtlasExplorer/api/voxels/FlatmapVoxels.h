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

#include <brayns/common/MathTypes.h>

#include <api/DataMangler.h>
#include <api/IVoxelList.h>

class FlatmapVoxels : public IVoxelList
{
public:
    FlatmapVoxels(const IDataMangler &dataMangler);
    bool isValidVoxel(size_t linealIndex) const override;
    int64_t getMinCoordinate();
    int64_t getMaxCoordinate();
    const std::vector<brayns::Vector2l> &getCoordinates() const noexcept;

private:
    std::vector<brayns::Vector2l> _voxels;
    int64_t _min;
    int64_t _max;
};
