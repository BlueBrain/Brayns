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

#include "SonataReportData.h"

#include <api/reports/common/FrameTimeCalculator.h>

namespace
{
inline static constexpr double sonataEpsilon = 1e-6;
}

namespace sonataloader
{
SonataReportData::SonataReportData(
    const std::string &reportPath,
    const std::string &population,
    bbp::sonata::Selection selection)
    : _reader(bbp::sonata::ElementReportReader(reportPath))
    , _population(_reader.openPopulation(population))
    , _selection(std::move(selection))
{
    auto [start, end, dt] = _population.getTimes();
    _start = static_cast<float>(start);
    _end = static_cast<float>(end);
    _dt = static_cast<float>(dt);
}

float SonataReportData::getStartTime() const noexcept
{
    return _start;
}

float SonataReportData::getEndTime() const noexcept
{
    return _end;
}

float SonataReportData::getTimeStep() const noexcept
{
    return _dt;
}

std::string SonataReportData::getTimeUnit() const noexcept
{
    return _population.getTimeUnits();
}

std::vector<float> SonataReportData::getFrame(uint32_t frameIndex) const
{
    auto times = _population.getTimes();
    auto start = static_cast<float>(std::get<0>(times));
    auto end = static_cast<float>(std::get<1>(times));
    auto dt = static_cast<float>(std::get<2>(times));

    auto time = FrameTimeCalculator::compute(frameIndex, start, end, dt);
    auto startTime = time - sonataEpsilon;
    auto endTime = startTime + dt;
    auto frame = _population.get(_selection, startTime, endTime);

    if (frame.data.empty())
    {
        throw std::runtime_error("Emtpy frame read");
    }

    return frame.data;
}
}
