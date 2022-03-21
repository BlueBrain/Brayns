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

#include <brayns/common/simulation/AbstractSimulationHandler.h>

// libsonata uses nonstd::optional which, if available, becomes std::optional
// however, libsonata is compiled enforcing c++14, so their type is always nonstd::optional
// then, symbol lookup errors happen
#define optional_CONFIG_SELECT_OPTIONAL optional_OPTIONAL_NONSTD
#include <bbp/sonata/report_reader.h>

namespace sonataloader
{
/**
 * @brief The SonataReportHandler class implements support to handle SONATA
 *        report simulations on the renderer for bloodflow radii simulation
 * type.
 */
class VasculatureRadiiHandler : public brayns::AbstractSimulationHandler
{
public:
    VasculatureRadiiHandler(
        const std::string &h5FilePath,
        const std::string &populationName,
        const bbp::sonata::Selection &selection);

    brayns::AbstractSimulationHandlerPtr clone() const final;

    std::vector<float> getFrameDataImpl(const uint32_t frame) final;

    const std::vector<float> &getCurrentRadiiFrame() const noexcept;
    size_t getRadiiFrameSize() const noexcept;
    void setLastUsedFrame(const uint32_t frame);
    uint32_t getLastUsedFrame() const noexcept;
    bool isReady() const final;

private:
    const std::string _h5FilePath;
    const std::string _populationName;
    const bbp::sonata::Selection _selection;
    const bbp::sonata::ElementReportReader _reader;
    const bbp::sonata::ElementReportReader::Population &_reportPopulation;

    bool _ready{true};

    size_t _radiiFrameSize;
    std::vector<float> _radii;
    uint32_t _lastRadiiFrame{std::numeric_limits<uint32_t>::max()};
};
} // namespace sonataloader
