/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <brayns/utils/MathTypes.h>

namespace
{
static inline constexpr double sonataEpsilon = 1e-6;
}

namespace sonataloader
{
SonataReportData::SonataReportData(
    const std::string &reportPath,
    const std::string &population,
    bbp::sonata::Selection selection):
    _reader(bbp::sonata::ElementReportReader(reportPath)),
    _population(_reader.openPopulation(population)),
    _selection(std::move(selection))
{
    auto [start, end, dt] = _population.getTimes();
    _start = start;
    _end = end;
    _dt = dt;
}

double SonataReportData::getStartTime() const noexcept
{
    return _start;
}

double SonataReportData::getEndTime() const noexcept
{
    return _end;
}

double SonataReportData::getTimeStep() const noexcept
{
    return _dt;
}

std::string SonataReportData::getTimeUnit() const noexcept
{
    return _population.getTimeUnits();
}

std::vector<float> SonataReportData::getFrame(double timestamp) const
{
    auto [start, end, dt] = _population.getTimes();
    timestamp = brayns::math::clamp(timestamp, start, end - dt);
    auto endTime = timestamp + dt;
    auto frame = _population.get(_selection, timestamp, endTime);

    if (frame.data.empty())
    {
        throw std::runtime_error("Emtpy frame read");
    }

    return frame.data;
}
}
