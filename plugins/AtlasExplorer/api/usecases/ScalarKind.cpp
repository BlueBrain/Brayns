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

#include "ScalarKind.h"

#include <brayns/engine/components/VolumeRendererComponent.h>
#include <brayns/engine/volumes/RegularVolume.h>

#include <api/kinds/common/DataFlipper.h>
#include <api/kinds/common/VolumeMeasures.h>

namespace
{
class DataGridAdjuster
{
public:
    template<typename T>
    static std::vector<uint8_t> adjust(std::vector<T> input, const brayns::Vector3f &sizes)
    {
        const auto flipped = DataFlipper::flipVertically<T>(sizes, std::move(input));

        std::vector<uint8_t> result(flipped.size() * sizeof(T));

        const auto src = flipped.data();
        auto dst = result.data();

        std::memcpy(dst, src, result.size());

        return result;
    }
};

class RegularVolumeBuilder
{
public:
    static brayns::RegularVolume build(const NRRDHeader &header, const INRRDData &data)
    {
        brayns::RegularVolume result;

        const auto measures = _computeMeasures(header);
        result.size = measures.sizes;
        result.spacing = measures.dimensions;

        const auto dataType = header.type;
        switch (dataType)
        {
        case NRRDType::UNSIGNED_CHAR:
            result.data = DataGridAdjuster::adjust(data.asBytes(), measures.sizes);
            result.dataType = brayns::VolumeDataType::UNSIGNED_CHAR;
            break;
        case NRRDType::CHAR:
        case NRRDType::SHORT:
            result.data = DataGridAdjuster::adjust(data.asShorts(), measures.sizes);
            result.dataType = brayns::VolumeDataType::SHORT;
            break;
        case NRRDType::UNSIGNED_SHORT:
            result.data = DataGridAdjuster::adjust(data.asUnsingedShorts(), measures.sizes);
            result.dataType = brayns::VolumeDataType::UNSIGNED_SHORT;
            break;
        case NRRDType::FLOAT:
            result.data = DataGridAdjuster::adjust(data.asFloats(), measures.sizes);
            result.dataType = brayns::VolumeDataType::FLOAT;
            break;
        case NRRDType::INT:
        case NRRDType::UNSIGNED_INT:
        case NRRDType::LONG:
        case NRRDType::UNSIGNED_LONG:
        case NRRDType::DOUBLE:
            result.data = DataGridAdjuster::adjust(data.asDoubles(), measures.sizes);
            result.dataType = brayns::VolumeDataType::DOUBLE;
            break;
        default:
            assert(false);
        }

        return result;
    };

private:
    static VolumeMeasures _computeMeasures(const NRRDHeader &header)
    {
        size_t offset = 0;
        if (header.dimensions == 4)
        {
            offset = 1;
        }

        return VolumeMeasuresComputer::compute(header, offset);
    }
};
}

void ScalarKind::initialize(const NRRDImage &image, brayns::Model &model) const
{
    const auto &header = image.getHeader();
    const auto &data = image.getData();
    auto volume = RegularVolumeBuilder::build(header, data);
    model.addComponent<brayns::VolumeRendererComponent<brayns::RegularVolume>>(std::move(volume));
}

void ScalarKind::handleUseCase(const NRRDImage &image, const UseCaseInfo &info, brayns::Model &model) const
{
    (void)image;
    (void)info;
    (void)model;
}
