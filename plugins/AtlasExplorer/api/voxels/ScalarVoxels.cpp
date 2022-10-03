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

#include "ScalarVoxels.h"

#include <api/utils/DataUtils.h>

#include <cassert>
#include <cmath>

ScalarVoxels::ScalarVoxels(const IDataMangler &data)
    : _data(data.asDoubles())
{
    auto minMax = DataMinMax::compute(_data);
    _min = minMax.first;
    _max = minMax.second;
}

bool ScalarVoxels::isValidVoxel(size_t linealIndex) const
{
    assert(linealIndex < _data.size());
    auto value = _data[linealIndex];
    return value > _min && std::isfinite(value);
}

double ScalarVoxels::getMinValue() const noexcept
{
    return _min;
}

double ScalarVoxels::getMaxValue() const noexcept
{
    return _max;
}

const std::vector<double> &ScalarVoxels::getValues() const noexcept
{
    return _data;
}
