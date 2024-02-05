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

#include <unordered_map>

namespace sonataloader
{
/**
 * @brief The SonataReportHandler class implements support to handle SONATA
 *        report simulations on the renderer for spike simulation types.
 */
class SonataSpikeHandler : public brayns::AbstractSimulationHandler
{
public:
    SonataSpikeHandler(const std::string& h5FilePath,
                       const std::string& populationName,
                       const bbp::sonata::Selection& selection);

    brayns::AbstractSimulationHandlerPtr clone() const final;

    std::vector<float> getFrameDataImpl(const uint32_t frame) final;

    bool isReady() const final { return _ready; }

private:
    const std::string _h5FilePath;
    const std::string _populationName;
    const bbp::sonata::Selection _selection;
    const bbp::sonata::SpikeReader _reader;
    const bbp::sonata::SpikeReader::Population& _spikePopulation;
    std::unordered_map<uint64_t, size_t> _gidsToIndex;
    bool _ready{true};
};
} // namespace sonataloader
