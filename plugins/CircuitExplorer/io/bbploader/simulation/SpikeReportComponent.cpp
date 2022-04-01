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

#include <brayns/engine/common/ExtractModelObject.h>
#include <brayns/engine/common/SizeHelper.h>
#include <brayns/engine/components/SimulationComponent.h>

#include <components/CircuitColorComponent.h>
#include <io/simulation/SimulationFrameIndexer.h>
#include <io/simulation/SimulationFrameTime.h>
#include <io/simulation/SimulationTransferFunction.h>

namespace
{
constexpr float DEFAULT_REST_VALUE = -80.f;
constexpr float DEFAULT_SPIKING_VALUE = -1.f;
constexpr float DEFAULT_TIME_INTERVAL = 0.01f;

std::unordered_map<uint32_t, size_t> getMapping(const brain::GIDSet &gids) noexcept
{
    std::unordered_map<uint32_t, size_t> result;
    size_t offset = 0;
    for(const auto gid : gids)
    {
        result[gid] = offset++;
    }
    return result;
}
}

namespace bbploader
{
SpikeReportComponent::SpikeReportComponent(std::unique_ptr<brain::SpikeReportReader> report,
                                           const brain::GIDSet &gids,
                                           const float spikeTimeInterval)
 : _report(std::move(report))
 , _gidIndexMap(getMapping(gids))
 , _spikeInterval(spikeTimeInterval)
{
}

size_t SpikeReportComponent::getSizeInBytes() const noexcept
{
    return sizeof(SpikeReportComponent);
}

void SpikeReportComponent::onStart()
{
    auto &model = getModel();
    auto &tf = brayns::ExtractModelObject::extractTransferFunction(model);
    SimulationTransferFunction::setUnipolarColormap(tf);

    float endTime = _report->getEndTime();
    model.addComponent<brayns::SimulationComponent>(0.f, endTime, DEFAULT_TIME_INTERVAL, "ms");
}

void SpikeReportComponent::onPreRender(const brayns::ParametersManager &parameters)
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
        tf.resetModified();
        tfChanged = true;
        _color = SimulationTransferFunction::sampleAsBuffer(tf);
    }
    const auto &animationParameters = parameters.getAnimationParameters();

    if(forceUpdate || tfChanged || animationParameters.isModified())
    {
        const auto frame = animationParameters.getFrame();
        const auto simEnd = _report->getEndTime();
        const auto frameTime = SimulationFrameTime::indexToTime(frame, 0.f, simEnd, DEFAULT_TIME_INTERVAL);
        const auto frameStart = frameTime - _spikeInterval;
        const auto frameEnd = frameTime + _spikeInterval;

        const auto spikes = _report->getSpikes(frameStart, frameEnd);
        const auto invInterval = 1.f / _spikeInterval;
        std::vector<float> values (_gidIndexMap.size(), DEFAULT_REST_VALUE);
        for (const auto &spike : spikes)
        {
            const auto gid = spike.second;
            const auto index = _gidIndexMap[gid];
            const auto spikeTime = spike.first;

            // Spike in the future - start growth
            if (spikeTime > frameTime)
            {
                auto alpha = (spikeTime - frameTime) * invInterval;
                alpha = std::min(std::max(0.f, alpha), 1.f);
                values[index] = DEFAULT_REST_VALUE * alpha + DEFAULT_SPIKING_VALUE * (1.0 - alpha);
            }
            // Spike in the past - start fading
            else if (spikeTime < frameTime)
            {
                auto alpha = (frameTime - spikeTime) * invInterval;
                alpha = std::min(std::max(0.f, alpha), 1.f);
                values[index] = DEFAULT_REST_VALUE * alpha + DEFAULT_SPIKING_VALUE * (1.0 - alpha);
            }
            // Spiking neuron
            else
            {
                values[index] = DEFAULT_SPIKING_VALUE;
            }
        }

        _indices = SimulationFrameIndexer::computeIndices(values, tf.getValuesRange());

        auto &colorComponent = model.getComponent<CircuitColorComponent>();
        auto &handler = colorComponent.getColorHandler();
        handler.updateIndexedColor(_color, _indices);
    }
}
}
