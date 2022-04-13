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

namespace dti
{
SpikeReportComponent::SpikeReportComponent(
    std::unique_ptr<brain::SpikeReportReader> report,
    std::unordered_map<uint64_t, std::vector<size_t>> gidStreamlineMap,
    float spikeDecayTime)
    : _report(std::move(report))
    , _gidStreamlineMap(std::move(gidStreamlineMap))
    , _spikeDecayTime(spikeDecayTime)
{
}

size_t SpikeReportComponent::getSizeInBytes() const noexcept
{
    size_t mapSize = 0;
    for (const auto &[gid, indexList] : _gidStreamlineMap)
    {
        mapSize += sizeof(uint64_t);
        mapSize += brayns::SizeHelper::vectorSize(indexList);
    }

    return sizeof(SpikeReportComponent) + mapSize + sizeof(brain::SpikeReportReader);
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

    if (!brayns::ExtractModelObject::isSimulationEnabled(model))
    {
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
}

}
