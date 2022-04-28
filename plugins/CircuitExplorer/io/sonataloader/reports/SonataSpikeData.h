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

#pragma once

#include <api/reports/IReportData.h>
#include <api/reports/common/SpikeUtils.h>

// libsonata uses nonstd::optional which, if available, becomes std::optional
// however, libsonata is compiled enforcing c++14, so their type is always nonstd::optional
// then, symbol lookup errors happen
#define optional_CONFIG_SELECT_OPTIONAL optional_OPTIONAL_NONSTD
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

    size_t getFrameSize() const noexcept override;

    float getStartTime() const noexcept override;

    float getEndTime() const noexcept override;

    float getTimeStep() const noexcept override;

    std::string getTimeUnit() const noexcept override;

    std::vector<float> getFrame(const uint32_t frameIndex) const override;

private:
    const bbp::sonata::SpikeReader _reader;
    const bbp::sonata::SpikeReader::Population &_population;
    const bbp::sonata::Selection _selection;
    const std::unordered_map<uint64_t, size_t> _mapping;
    const SpikeCalculator _calculator;
    const float _interval{};
    float _start{};
    float _end{};
};
}
