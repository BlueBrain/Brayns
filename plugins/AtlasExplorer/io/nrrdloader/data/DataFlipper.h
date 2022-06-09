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

#include <io/nrrdloader/NRRDHeader.h>
#include <io/nrrdloader/header/HeaderUtils.h>

#include <vector>

class DataFlipper
{
public:
    template<typename T>
    static std::vector<T> flipVertically(const NRRDHeader &header, const std::vector<T> &input) noexcept
    {
        const auto sizes = HeaderUtils::get3DSize(header);
        const auto voxelDimension = HeaderUtils::getVoxelDimension(header);

        const auto width = sizes.x;
        const auto height = sizes.y;
        const auto depth = sizes.z;

        const auto rowSize = width * voxelDimension;
        const auto frameSize = height * rowSize;

        std::vector<T> result;
        result.reserve(input.size());

        for (size_t i = 0; i < depth; ++i)
        {
            const auto sliceStart = frameSize * i;

            for (size_t j = 0; j < height; ++j)
            {
                const auto rowCopyStart = sliceStart + frameSize - rowSize * (j + 1);
                const auto rowCopyEnd = rowCopyStart + rowSize;

                auto begin = input.begin();
                std::advance(begin, rowCopyStart);
                auto end = input.begin();
                std::advance(end, rowCopyEnd);
                result.insert(result.end(), begin, end);
            }
        }

        return result;
    }
};
