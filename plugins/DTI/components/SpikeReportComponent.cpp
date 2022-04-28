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

#include "SpikeReportComponent.h"

#include <brayns/engine/Model.h>
#include <brayns/engine/common/ExtractModelObject.h>
#include <brayns/engine/common/SizeHelper.h>
#include <brayns/engine/components/SimulationComponent.h>

#include <components/DTIComponent.h>

namespace
{
struct SpikeFrameProcessor
{
    SpikeFrameProcessor(
        const std::unordered_map<uint64_t, std::vector<size_t>> &gidStreamlineMap,
        brain::SpikeReportReader &reader,
        float invDecayTime)
        : _gidStreamlineMap(gidStreamlineMap)
        , _reader(reader)
        , _invDecayTime(invDecayTime)
    {
    }

    std::vector<std::vector<float>> process(size_t numStreamlines, float frameTimestamp)
    {
        std::vector<std::vector<float>> data(numStreamlines);

        constexpr auto dt = 0.01f;
        const auto spikes = _reader.getSpikes(frameTimestamp, frameTimestamp + dt);

        for (size_t i = 0; i < spikes.size(); ++i)
        {
            const auto &spike = spikes[i];
            const auto spikeTime = spike.first;
            const auto gid = spike.second;

            const auto normalizedSpikeTime = std::max(0.f, spikeTime - frameTimestamp);
            const auto normalizedSpikeLife = normalizedSpikeTime * _invDecayTime;
            // Spike visualization is over
            if (normalizedSpikeLife > 1.f)
            {
                continue;
            }

            auto it = _gidStreamlineMap.find(gid);
            if (it == _gidStreamlineMap.end())
            {
                continue;
            }

            auto &affectedStreamlines = it->second;
            if (affectedStreamlines.empty())
            {
                continue;
            }

            for (const auto streamlineIndex : affectedStreamlines)
            {
                auto &streamlineBuffer = data[streamlineIndex];
                streamlineBuffer.push_back(normalizedSpikeLife);
            }
        }

        return data;
    }

private:
    const std::unordered_map<uint64_t, std::vector<size_t>> _gidStreamlineMap;
    brain::SpikeReportReader &_reader;
    float _invDecayTime{};
};
}

namespace dti
{
SpikeReportComponent::SpikeReportComponent(
    std::unique_ptr<brain::SpikeReportReader> report,
    std::unordered_map<uint64_t, std::vector<size_t>> gidStreamlineMap,
    float spikeDecayTime)
    : _report(std::move(report))
    , _gidStreamlineMap(std::move(gidStreamlineMap))
    , _invSpikeDecayTime(1.f / spikeDecayTime)
{
}

void SpikeReportComponent::onStart()
{
    auto &model = getModel();

    const auto startTime = 0.f;
    const auto endTime = _report->getEndTime();
    const auto dt = 0.01f;

    model.addComponent<brayns::SimulationComponent>(startTime, endTime, dt, "");
}

void SpikeReportComponent::onPreRender(const brayns::ParametersManager &parameters)
{
    auto &model = getModel();
    auto &dti = model.getComponent<DTIComponent>();

    if (!brayns::ExtractModelObject::isSimulationEnabled(model))
    {
        // First onPreRender after disabling simulation - restore default colors
        if (_lastEnabledValue)
        {
            dti.setDefaultColors();
        }

        _lastEnabledValue = false;
        return;
    }

    bool forceUpdate = !_lastEnabledValue;
    _lastEnabledValue = true;

    const auto &animationParameters = parameters.getAnimationParameters();
    forceUpdate = forceUpdate || animationParameters.isModified();

    if (!forceUpdate)
    {
        return;
    }

    const auto numStreamlines = dti.getNumStreamlines();

    constexpr auto dt = 0.01f;
    const auto endTime = _report->getEndTime();
    const auto frameIndex = animationParameters.getFrame();
    const auto frameTimestamp = std::clamp(frameIndex * dt, 0.f, endTime);

    SpikeFrameProcessor processor(_gidStreamlineMap, *_report, _invSpikeDecayTime);
    const auto data = processor.process(numStreamlines, frameTimestamp);

    dti.updateSimulation(data);
}

}
