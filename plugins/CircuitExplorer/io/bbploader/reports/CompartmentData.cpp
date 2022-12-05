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

namespace bbploader
{
CompartmentData::CompartmentData(std::unique_ptr<brion::CompartmentReport> report)
    : _report(std::move(report))
{
}

float CompartmentData::getStartTime() const noexcept
{
    return static_cast<float>(_report->getStartTime());
}

float CompartmentData::getEndTime() const noexcept
{
    return static_cast<float>(_report->getEndTime());
}

float CompartmentData::getTimeStep() const noexcept
{
    return static_cast<float>(_report->getTimestep());
}

std::string CompartmentData::getTimeUnit() const noexcept
{
    return _report->getTimeUnit();
}

std::vector<float> CompartmentData::getFrame(double timestamp) const
{
<<<<<<< HEAD
    auto start = getStartTime();
    auto end = getEndTime();
    auto dt = getTimeStep();
    auto frameTime = FrameTimeCalculator::compute(frameIndex, start, end, dt);

    auto frameFuture = _report->loadFrame(frameTime);
=======
    auto frameFuture = _report->loadFrame(timestamp);
>>>>>>> Checkpoint
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
    auto mapping = std::vector<CellReportMapping>();
    mapping.reserve(gids.size());

    for (size_t i = 0; i < gids.size(); ++i)
    {
        auto &count = ccounts[i];
        auto &offset = offsets[i];

        auto &cellMapping = mapping.emplace_back();

        cellMapping.globalOffset = offset[0];
        cellMapping.compartments = count;
        cellMapping.offsets.reserve(offset.size());

        for (size_t j = 0; j < offset.size(); ++j)
        {
            auto total = offset[j] - cellMapping.globalOffset;
            cellMapping.offsets.push_back(static_cast<uint16_t>(total));
        }
    }

    return mapping;
}
}
