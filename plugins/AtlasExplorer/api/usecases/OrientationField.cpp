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

#include "OrientationField.h"

#include <components/RotationVolumeComponent.h>

namespace
{
class QuaternionExtractor
{
public:
    static std::vector<brayns::Quaternion> extract(const std::vector<float> &data)
    {
        std::vector<brayns::Quaternion> result(data.size() / 4, brayns::Quaternion(0.f, 0.f, 0.f, 0.f));

#pragma omp parallel for
        for (size_t i = 0; i < result.size(); ++i)
        {
            const auto index = i * 4;
            brayns::Vector4f qVector(data[index], data[index + 1], data[index + 2], data[index + 3]);
            if (glm::length(qVector) == 0.f)
            {
                continue;
            }
            auto areFinite = glm::isfinite(qVector);
            if (!glm::compMin(areFinite))
            {
                continue;
            }
            // NRRD quaternions are stored as wxyz
            result[i] = brayns::Quaternion(qVector.x, qVector.y, qVector.z, qVector.w);
        }

        return result;
    }
};
}

std::string OrientationField::getName() const
{
    return "Orientation field";
}

bool OrientationField::isVolumeValid(const AtlasVolume &volume) const
{
    return volume.getVoxelSize() == 4;
}

std::unique_ptr<brayns::Model> OrientationField::execute(const AtlasVolume &volume, const brayns::JsonValue &payload)
    const
{
    (void)payload;

    const auto &data = volume.getData();
    const auto floats = data.asFloats();
    const auto quaternions = QuaternionExtractor::extract(floats);
    const auto &size = volume.getSize();
    const auto &spacing = volume.getSpacing();

    auto model = std::make_unique<brayns::Model>();
    model->addComponent<RotationVolumeComponent>(size, spacing, quaternions);

    return model;
}
