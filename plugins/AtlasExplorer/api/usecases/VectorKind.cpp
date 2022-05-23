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

#include "VectorKind.h"

#include <api/kinds/common/VolumeMeasures.h>
#include <components/VectorVolumeComponent.h>

#include <cstring>

namespace
{
class DataToVector
{
public:
    static std::vector<brayns::Vector3f> convert(size_t vectorDimension, const std::vector<float> &data)
    {
        assert(vectorDimension <= 3);

        size_t numElements = data.size() / vectorDimension;
        assert(numElements * 3 == data.size());

        std::vector<brayns::Vector3f> vectors;
        vectors.reserve(numElements);

        for (size_t i = 0; i < numElements; ++i)
        {
            auto &vector = vectors.emplace_back(0.f);

            const auto dataIndex = i * vectorDimension;
            for (size_t j = 0; j < vectorDimension; ++j)
            {
                vector[j] = data[dataIndex + j];
            }
        }

        return vectors;
    }
};
}

void VectorKind::initialize(const NRRDImage &image, brayns::Model &model) const
{
    const auto &header = image.getHeader();
    const auto &data = image.getData();
    const auto &sizes = header.sizes;
    const auto vectorDimension = static_cast<size_t>(sizes.front());
    const auto floatData = data.asFloats();
    const auto vectors = DataToVector::convert(vectorDimension, floatData);
    const auto measures = VolumeMeasuresComputer::compute(header, 1);
    model.addComponent<VectorVolumeComponent>(measures.sizes, measures.dimensions, vectors);
}

void VectorKind::handleUseCase(const NRRDImage &image, const UseCaseInfo &info, brayns::Model &model) const
{
    (void)image;
    (void)info;
    (void)model;
}
