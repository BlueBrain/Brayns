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

#include "RotationKind.h"

#include <components/RotationVolumeComponent.h>
#include <io/nrrdloader/kinds/common/DataFlipper.h>
#include <io/nrrdloader/kinds/common/VolumeMeasures.h>

#include <cstring>

namespace
{
class DataToQuaternion
{
public:
    static std::vector<brayns::Quaternion> convert(const std::vector<float> &data)
    {
        // NRRD stores quaternions as (w,x,y,z)
        std::vector<brayns::Quaternion> quaternions(data.size() / 4);
        assert(quaternions.size() * 4 == data.size());

        const auto src = data.data();
        auto dst = quaternions.data();

        std::memcpy(dst, src, data.size() * sizeof(float));

        return quaternions;
    }
};
}

void RotationKind::createComponent(const NRRDHeader &header, const INRRDData &data, brayns::Model &model) const
{
    const auto floatData = data.asFloats();
    const auto measures = VolumeMeasuresComputer::compute(header, 1);
    auto quaternionsData = DataToQuaternion::convert(floatData);
    const auto quaternions = DataFlipper::flipVertically(measures.sizes, std::move(quaternionsData));
    model.addComponent<RotationVolumeComponent>(measures.sizes, measures.dimensions, quaternions);
}
