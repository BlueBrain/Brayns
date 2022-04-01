/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
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

#include "CompartmentReportComponent.h"

#include <brayns/engine/common/ExtractModelObject.h>
#include <brayns/engine/common/SizeHelper.h>
#include <brayns/engine/components/SimulationComponent.h>

#include <components/CircuitColorComponent.h>
#include <io/simulation/SimulationFrameIndexer.h>
#include <io/simulation/SimulationFrameTime.h>
#include <io/simulation/SimulationTransferFunction.h>

namespace
{
std::vector<SimulationMapping> getMapping(brion::CompartmentReport &report)
{
    const auto &ccounts = report.getCompartmentCounts();
    const auto &offsets = report.getOffsets();

    const auto &gids = report.getGIDs();
    std::vector<SimulationMapping> mapping(gids.size());

#pragma omp parallel for
    for (size_t i = 0; i < gids.size(); ++i)
    {
        const auto &count = ccounts[i];
        const auto &offset = offsets[i];

        mapping[i].globalOffset = offset[0];

        mapping[i].compartments = std::vector<uint16_t>(count.begin(), count.end());

        mapping[i].offsets.resize(offset.size());
        for (size_t j = 0; j < offset.size(); ++j)
            mapping[i].offsets[j] = offset[j] - mapping[i].globalOffset;
    }

    return mapping;
}
}

namespace bbploader
{
CompartmentReportComponent::CompartmentReportComponent(std::unique_ptr<brion::CompartmentReport> report,
                                                       const std::vector<CompartmentStructure> &compartments)
 : _report(std::move(report))
 , _offsets(SimulationMappingGenerator::generate(compartments, getMapping(*_report)))
{
}

size_t CompartmentReportComponent::getSizeInBytes() const noexcept
{
    return sizeof(CompartmentReportComponent)
            + brayns::SizeHelper::vectorSize(_offsets)
            + brayns::SizeHelper::vectorSize(_indices);
}

void CompartmentReportComponent::onStart()
{
    auto &model = getModel();
    auto &tf = brayns::ExtractModelObject::extractTransferFunction(model);
    SimulationTransferFunction::setUnipolarColormap(tf);

    float startTime = _report->getStartTime();
    float endTime = _report->getEndTime();
    float dt = _report->getTimestep();
    std::string timeUnit = _report->getTimeUnit();
    model.addComponent<brayns::SimulationComponent>(startTime, endTime, dt, std::move(timeUnit));
}

void CompartmentReportComponent::onPreRender(const brayns::ParametersManager &parameters)
{
    auto &model = getModel();

    auto &simulationComponent = model.getComponent<brayns::SimulationComponent>();
    if(!simulationComponent.enabled())
    {
        _lastEnabledValue = false;
        return;
    }

    bool forceUpdate = !_lastEnabledValue;
    _lastEnabledValue = true;

    bool tfChanged = false;
    auto &tf = brayns::ExtractModelObject::extractTransferFunction(model);
    if(tf.isModified())
    {
        _colors = SimulationTransferFunction::sampleAsBuffer(tf);
        tf.resetModified();
        tfChanged = true;
    }

    const auto &animationParameters = parameters.getAnimationParameters();
    if(forceUpdate || tfChanged || animationParameters.isModified())
    {
        const auto frame = animationParameters.getFrame();
        const auto simStart = _report->getStartTime();
        const auto simEnd = _report->getEndTime();
        const auto simDt = _report->getTimestep();
        const auto frameTime = SimulationFrameTime::indexToTime(frame, simStart, simEnd, simDt);
        auto frameFuture = _report->loadFrame(frameTime);
        auto simulationFrame = frameFuture.get();
        auto &frameData = *simulationFrame.data;

        auto &tfRange = tf.getValuesRange();

        _indices = SimulationFrameIndexer::computeIndices(frameData, _offsets, tfRange);

        auto &colorComponent = model.getComponent<CircuitColorComponent>();
        auto &handler = colorComponent.getColorHandler();
        handler.updateIndexedColor(_colors, _indices);
    }
}
}
