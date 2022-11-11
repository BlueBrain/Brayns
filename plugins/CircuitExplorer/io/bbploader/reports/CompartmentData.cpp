/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman <nadir.romanguerrero@epfl.ch>
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

#include "CompartmentData.h"

#include <api/reports/common/FrameTimeCalculator.h>

namespace bbploader
{
CompartmentData::CompartmentData(std::unique_ptr<brion::CompartmentReport> report)
    : _report(std::move(report))
{
}

float CompartmentData::getStartTime() const noexcept
{
    return _report->getStartTime();
}

float CompartmentData::getEndTime() const noexcept
{
    return _report->getEndTime();
}

float CompartmentData::getTimeStep() const noexcept
{
    return _report->getTimestep();
}

std::string CompartmentData::getTimeUnit() const noexcept
{
    return _report->getTimeUnit();
}

std::vector<float> CompartmentData::getFrame(const uint32_t frameIndex) const
{
    auto start = _report->getStartTime();
    auto end = _report->getEndTime();
    auto dt = _report->getTimestep();
    auto frameTime = FrameTimeCalculator::compute(frameIndex, start, end, dt);

    auto frameFuture = _report->loadFrame(frameTime);
    auto frame = frameFuture.get();
    auto &data = frame.data;

    if (!data || data->empty())
    {
        throw std::runtime_error("Null report frame read");
    }

    return *data;
}

std::vector<CellReportMapping> CompartmentData::computeMapping() const noexcept
{
    auto &ccounts = _report->getCompartmentCounts();
    auto &offsets = _report->getOffsets();

    auto &gids = _report->getGIDs();
    std::vector<CellReportMapping> mapping(gids.size());

    for (size_t i = 0; i < gids.size(); ++i)
    {
        auto &count = ccounts[i];
        auto &offset = offsets[i];

        auto &cellMapping = mapping[i];
        cellMapping.globalOffset = offset[0];
        cellMapping.compartments = std::vector<uint16_t>(count.begin(), count.end());
        cellMapping.offsets.resize(offset.size());

        for (size_t j = 0; j < offset.size(); ++j)
        {
            cellMapping.offsets[j] = offset[j] - cellMapping.globalOffset;
        }
    }

    return mapping;
}
}
