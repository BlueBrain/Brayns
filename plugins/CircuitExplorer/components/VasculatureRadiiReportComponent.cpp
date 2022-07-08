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

#include "VasculatureRadiiReportComponent.h"

#include <brayns/engine/Model.h>
#include <brayns/engine/common/ExtractModelObject.h>
#include <brayns/engine/components/SimulationComponent.h>

#include <components/VasculatureComponent.h>

namespace
{
struct OriginalRadiiManager
{
    static std::vector<float> save(brayns::Geometry<brayns::Primitive> &geometry)
    {
        const auto &primitives = geometry.getPrimitives();
        std::vector<float> result(primitives.size() * 2);

        for (size_t i = 0; i < primitives.size(); ++i)
        {
            const auto index = i * 2;
            const auto &primitive = primitives[i];

            result[index] = primitive.r0;
            result[index + 1] = primitive.r1;
        }

        return result;
    }

    static void restore(brayns::Geometry<brayns::Primitive> &geometry, const std::vector<float> &originalRadii)
    {
        geometry.forEach(
            [&](uint32_t i, brayns::Primitive &primitive)
            {
                const auto index = i * 2;
                primitive.r0 = originalRadii[index];
                primitive.r1 = originalRadii[index + 1];
            });
    }
};

struct RadiiReportUpdater
{
    static void update(
        brayns::Geometry<brayns::Primitive> &geometry,
        const std::vector<size_t> offsets,
        const std::vector<float> frame)
    {
        geometry.forEach(
            [&](uint32_t i, brayns::Primitive &primitive)
            {
                const auto offset = offsets[i];
                primitive.r0 = frame[offset];
                primitive.r1 = frame[offset];
            });
    }
};
}

VasculatureRadiiReportComponent::VasculatureRadiiReportComponent(
    std::unique_ptr<IReportData> data,
    std::vector<size_t> offsets)
    : _report(std::move(data))
    , _offsets(std::move(offsets))
{
}

void VasculatureRadiiReportComponent::onCreate()
{
    auto &model = getModel();

    auto &vasculature = model.getComponent<VasculatureComponent>();
    auto &geometry = vasculature.getGeometry();
    _originalRadii = OriginalRadiiManager::save(geometry);

    const auto startTime = _report->getStartTime();
    const auto endTime = _report->getEndTime();
    const auto dt = _report->getTimeStep();
    auto timeUnit = _report->getTimeUnit();
    model.addComponent<brayns::SimulationComponent>(startTime, endTime, dt, std::move(timeUnit));
}

void VasculatureRadiiReportComponent::onPreRender(const brayns::ParametersManager &parameters)
{
    auto &model = getModel();
    auto &vasculature = model.getComponent<VasculatureComponent>();
    auto &geometry = vasculature.getGeometry();

    if (!brayns::ExtractModelObject::isSimulationEnabled(model))
    {
        // If we just disabled simulation, restore original radii
        if (_lastEnabledValue)
        {
            OriginalRadiiManager::restore(geometry, _originalRadii);
        }
        _lastEnabledValue = false;
        return;
    }

    bool forceUpdate = !_lastEnabledValue;
    _lastEnabledValue = true;

    const auto &simulation = parameters.getSimulationParameters();
    forceUpdate = forceUpdate || simulation.getModifiedFlag();

    if (forceUpdate)
    {
        const auto frameIndex = simulation.getFrame();
        const auto frameData = _report->getFrame(frameIndex);
        RadiiReportUpdater::update(geometry, _offsets, frameData);
    }
}
