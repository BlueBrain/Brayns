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

#include "ReportComponent.h"

#include <brayns/engine/common/ExtractComponent.h>
#include <brayns/engine/components/ColorRampComponent.h>
#include <brayns/engine/components/SimulationComponent.h>
#include <brayns/engine/model/Model.h>

#include <api/reports/ColorRampUtils.h>
#include <components/CircuitColorComponent.h>

ReportComponent::ReportComponent(std::unique_ptr<IReportData> data, std::unique_ptr<IColormapIndexer> indexer)
    : _report(std::move(data))
    , _indexer(std::move(indexer))
{
}

void ReportComponent::onCreate()
{
    auto &model = getModel();

    auto colorRamp = ColorRampUtils::createUnipolarColorRamp();
    model.addComponent<brayns::ColorRampComponent>(std::move(colorRamp));

    const auto startTime = _report->getStartTime();
    const auto endTime = _report->getEndTime();
    const auto dt = _report->getTimeStep();
    auto timeUnit = _report->getTimeUnit();
    model.addComponent<brayns::SimulationComponent>(startTime, endTime, dt, std::move(timeUnit));
}

void ReportComponent::onPreRender(const brayns::ParametersManager &parameters)
{
    auto &model = getModel();

    if (!brayns::ExtractComponent::simulationEnabled(model))
    {
        _lastEnabledValue = false;
        return;
    }

    auto &colorRamp = brayns::ExtractComponent::colorRamp(model);
    auto colorRampDirty = colorRamp.isModified();
    colorRamp.resetModified();

    auto &simulation = parameters.getSimulationParameters();
    auto simulationModified = simulation.isModified();

    auto forceUpdate = !_lastEnabledValue || simulationModified || colorRampDirty;
    _lastEnabledValue = true;

    if (forceUpdate)
    {
        auto colors = ColorRampUtils::createSampleBuffer(colorRamp);

        const auto frameIndex = simulation.getFrame();
        const auto frameData = _report->getFrame(frameIndex);
        const auto &range = colorRamp.getValuesRange();
        auto indices = _indexer->generate(frameData, range);

        auto &colorComponent = model.getComponent<CircuitColorComponent>();
        auto &handler = colorComponent.getColorHandler();
        handler.updateIndexedColor(colors, indices);
    }
}
