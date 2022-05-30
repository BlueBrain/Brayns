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

#include "AtlasVolumeBuilder.h"

#include <api/atlasvolumes/OrientationVolume.h>
#include <api/atlasvolumes/ScalarVolume.h>
#include <io/nrrdloader/header/HeaderUtils.h>

#include <cassert>

namespace
{
class QuaternionBuilder
{
public:
    static std::vector<brayns::Quaternion> build(const IDataMangler &data)
    {
        const auto floats = data.asFloats();
        const auto numElements = floats.size() / 4;
        assert(numElements * 4 == floats.size());

        std::vector<brayns::Quaternion> quaternions;
        quaternions.reserve(numElements);

        for (size_t i = 0; i < numElements; ++i)
        {
            const auto j = i * 4;
            auto w = floats[j];
            auto x = floats[j + 1];
            auto y = floats[j + 2];
            auto z = floats[j + 3];
            auto q = brayns::Quaternion(w, x, y, z);

            if (!_isValid(q))
            {
                quaternions.emplace_back(0.f, 0.f, 0.f, 0.f);
                continue;
            }

            quaternions.push_back(q);
        }

        return quaternions;
    }

private:
    static bool _isValid(const brayns::Quaternion &quaternion)
    {
        for (size_t i = 0; i < 4; ++i)
        {
            if (quaternion[i] != 0.f && !std::isnormal(quaternion[i]))
            {
                return false;
            }
        }

        return true;
    }
};
}

std::unique_ptr<IAtlasVolume> AtlasVolumeBuilder::build(const NRRDHeader &header, std::unique_ptr<IDataMangler> data)
{
    const auto voxelDimension = HeaderUtils::getVoxelDimension(header);
    const auto gridSize = HeaderUtils::get3DSize(header);
    const auto gridSpacing = HeaderUtils::get3DDimensions(header);

    if (voxelDimension == 1)
    {
        return std::make_unique<ScalarVolume>(gridSize, gridSpacing, std::move(data));
    }

    if (voxelDimension == 4)
    {
        auto quaternions = QuaternionBuilder::build(*data);
        return std::make_unique<OrientationVolume>(gridSize, gridSpacing, std::move(quaternions));
    }

    throw std::runtime_error("Only scalar and quaternion volumes are handled");
}
