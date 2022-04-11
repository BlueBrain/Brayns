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
    const auto [start, end, dt] = _population.getTimes();
    _start = start;
    _end = end;
    _dt = dt;
    const auto firstFrame = getFrame(0);
    _frameSize = firstFrame.size();
}

size_t SonataReportData::getFrameSize() const noexcept
{
    return _frameSize;
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

std::vector<float> SonataReportData::getFrame(const uint32_t frameIndex) const
{
    const auto [start, end, dt] = _population.getTimes();
    const auto timestamp = FrameTimeCalculator::compute(frameIndex, start, end, dt);
    const auto frame = _population.get(
        nonstd::optional<bbp::sonata::Selection>(_selection),
        nonstd::optional<double>(timestamp),
        nonstd::nullopt,
        nonstd::nullopt);

    return frame.data;
}
}
