/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include "SpikeReportSystem.h"

#include <utility>

#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/components/GeometryViews.h>
#include <brayns/engine/components/SimulationInfo.h>
#include <brayns/engine/geometry/types/Capsule.h>

#include <api/StreamlineColorGenerator.h>
#include <components/SpikeReportData.h>
#include <components/StreamlineColors.h>

#include <ospray/ospray_cpp/ext/rkcommon.h>

namespace
{
/**
 * @brief Applies default Streamline colors to the geometry view
 */
class DefaultColor
{
public:
    static void restore(brayns::Components &components)
    {
        auto &geometries = components.get<brayns::Geometries>();
        auto &views = components.get<brayns::GeometryViews>();
        auto &colors = components.get<dti::StreamlineColors>();

        for (size_t i = 0; i < geometries.elements.size(); ++i)
        {
            auto capsules = geometries.elements[i].as<brayns::Capsule>();

            auto &color = colors.elements[i];
            color = dti::StreamlineColorGenerator::generate(*capsules);

            auto &view = views.elements[i];
            view.setColorPerPrimitive(ospray::cpp::SharedData(color));
        }
        views.modified = true;
    }
};

/**
 * @brief Applies a spike highlight to the streamline's geometry view
 */
class SimulationColorPainter
{
public:
    static void paint(brayns::Components &components, const std::vector<std::vector<float>> &streamlineValues)
    {
        auto &views = components.get<brayns::GeometryViews>();
        auto &geometries = components.get<brayns::Geometries>();
        auto &colors = components.get<dti::StreamlineColors>();

        for (size_t i = 0; i < streamlineValues.size(); ++i)
        {
            auto &streamlineData = streamlineValues[i];
            if (streamlineData.empty())
            {
                continue;
            }

            auto &geometry = geometries.elements[i];
            auto &color = colors.elements[i];
            auto &view = views.elements[i];

            auto primitiveCount = geometry.numPrimitives();
            auto streamlineLength = primitiveCount - 1;

            for (auto value : streamlineData)
            {
                auto index = static_cast<size_t>(value * streamlineLength);
                index = std::clamp(index, 0ul, streamlineLength);
                color[index] = brayns::Vector4f(1.f);
                view.setColorPerPrimitive(ospray::cpp::SharedData(color));
            }
        }
        views.modified = true;
    }
};

/**
 * @brief Uses the spike value to generate a normalized index that will be used to know
 * which primitive of each streamline to update
 */
class SpikeFrameProcessor
{
public:
    static std::vector<std::vector<float>> process(const dti::SpikeReportData &data, float frameTime)
    {
        auto &spikes = data.spikes;
        auto invDecayTime = 1.f / data.decayTime;
        auto &gidMap = data.gidToFibers;

        std::vector<std::vector<float>> result(data.numStreamlines);

        for (size_t i = 0; i < spikes.size(); ++i)
        {
            auto &spike = spikes[i];
            if (spike.time < frameTime)
            {
                continue;
            }

            auto normalizedSpikeLife = std::max(0.f, (spike.time - frameTime) * invDecayTime);
            // Spike visualization is over
            if (normalizedSpikeLife > 1.f)
            {
                continue;
            }

            auto it = gidMap.find(spike.gid);
            if (it == gidMap.end())
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
                auto &streamlineBuffer = result[streamlineIndex];
                streamlineBuffer.push_back(normalizedSpikeLife);
            }
        }

        return result;
    }
};

}

namespace dti
{
bool SpikeReportSystem::isEnabled(brayns::Components &components)
{
    auto &info = components.get<brayns::SimulationInfo>();
    if (info.enabled)
    {
        return true;
    }

    auto &data = components.get<SpikeReportData>();
    if (std::exchange(data.lastEnabledFlag, false))
    {
        DefaultColor::restore(components);
    }

    return false;
}

bool SpikeReportSystem::shouldExecute(brayns::Components &components)
{
    auto &data = components.get<SpikeReportData>();
    return !std::exchange(data.lastEnabledFlag, true);
}

void SpikeReportSystem::execute(brayns::Components &components, double frameTimestamp)
{
    auto &info = components.get<brayns::SimulationInfo>();
    auto &spikeData = components.get<dti::SpikeReportData>();
    auto data = SpikeFrameProcessor::process(spikeData, frameTimestamp);
    SimulationColorPainter::paint(components, data);
}
}
