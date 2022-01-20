/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
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

#include "VasculatureRadiiHandler.h"

namespace sonataloader
{
namespace
{
auto frameIndexToTimestamp(const uint32_t frame, const double dt) noexcept
{
    return static_cast<double>(std::nextafter(dt, INFINITY) * frame);
}
} // namespace

VasculatureRadiiHandler::VasculatureRadiiHandler(
    const std::string& h5FilePath, const std::string& populationName,
    const bbp::sonata::Selection& selection)
    : brayns::AbstractSimulationHandler()
    , _h5FilePath(h5FilePath)
    , _populationName(populationName)
    , _selection(selection)
    , _reader(bbp::sonata::ElementReportReader(h5FilePath))
    , _reportPopulation(_reader.openPopulation(populationName))
{
    const auto times = _reportPopulation.getTimes();
    _startTime = std::get<0>(times);
    _endTime = std::get<1>(times);
    _dt = std::get<2>(times);
    _unit = _reportPopulation.getTimeUnits();

    auto firstFrame =
        _reportPopulation.get(selection, _startTime, _startTime + _dt);
    _frameSize = 0;
    _radiiFrameSize = firstFrame.ids.size();
    _radii.resize(_radiiFrameSize);
    _nbFrames = static_cast<uint32_t>(
        std::round((std::nextafter(_endTime, INFINITY) -
                    std::nextafter(_startTime, INFINITY)) /
                   _dt));
}

brayns::AbstractSimulationHandlerPtr VasculatureRadiiHandler::clone() const
{
    return std::make_shared<VasculatureRadiiHandler>(_h5FilePath,
                                                     _populationName,
                                                     _selection);
}

std::vector<float> VasculatureRadiiHandler::getFrameDataImpl(
    const uint32_t frame)
{
    _ready = false;
    const auto realFrame = frame > _nbFrames ? _nbFrames : frame;
    const auto timestamp = frameIndexToTimestamp(realFrame, _dt);
    _radii = _reportPopulation.get(_selection, timestamp).data;
    _ready = true;

    return {};
}

const std::vector<float>& VasculatureRadiiHandler::getCurrentRadiiFrame()
    const noexcept
{
    return _radii;
}
} // namespace sonataloader
