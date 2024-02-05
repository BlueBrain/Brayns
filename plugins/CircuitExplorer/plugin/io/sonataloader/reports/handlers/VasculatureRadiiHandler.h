/* Copyright 2015-2024 Blue Brain Project/EPFL
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
    VasculatureRadiiHandler(const std::string& h5FilePath,
                            const std::string& populationName,
                            const bbp::sonata::Selection& selection);

    brayns::AbstractSimulationHandlerPtr clone() const final;

    std::vector<float> getFrameDataImpl(const uint32_t frame) final;

    const std::vector<float>& getCurrentRadiiFrame() const noexcept;

    size_t getRadiiFrameSize() const noexcept { return _radiiFrameSize; }

    void setLastUsedFrame(const uint32_t frame) { _lastRadiiFrame = frame; }

    uint32_t getLastUsedFrame() const noexcept { return _lastRadiiFrame; }

    bool isReady() const final { return _ready; }

private:
    const std::string _h5FilePath;
    const std::string _populationName;
    const bbp::sonata::Selection _selection;
    const bbp::sonata::ElementReportReader _reader;
    const bbp::sonata::ElementReportReader::Population& _reportPopulation;

    bool _ready{true};

    size_t _radiiFrameSize;
    std::vector<float> _radii;
    uint32_t _lastRadiiFrame{std::numeric_limits<uint32_t>::max()};
};
} // namespace sonataloader
