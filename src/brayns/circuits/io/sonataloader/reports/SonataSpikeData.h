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

#pragma once

#include <brayns/circuits/api/reports/IReportData.h>
#include <brayns/circuits/api/reports/SpikeUtils.h>

#include <bbp/sonata/report_reader.h>

namespace sonataloader
{
class SonataSpikeData final : public IReportData
{
public:
    SonataSpikeData(
        const std::string &reportPath,
        const std::string &population,
        bbp::sonata::Selection selection,
        float interval);

    double getStartTime() const noexcept override;
    double getEndTime() const noexcept override;
    double getTimeStep() const noexcept override;
    std::string getTimeUnit() const noexcept override;
    std::vector<float> getFrame(double timestamp) const override;

private:
    const bbp::sonata::SpikeReader _reader;
    const bbp::sonata::SpikeReader::Population &_population;
    const bbp::sonata::Selection _selection;
    const std::unordered_map<uint64_t, size_t> _mapping;
    const SpikeCalculator _calculator;
    float _interval{};
    double _start{};
    double _end{};
};
}
