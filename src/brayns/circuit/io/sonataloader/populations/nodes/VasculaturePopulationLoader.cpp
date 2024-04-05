/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include "VasculaturePopulationLoader.h"

#include <brayns/core/engine/components/Geometries.h>
#include <brayns/core/engine/geometry/types/Capsule.h>

#include <brayns/circuit/api/circuit/VasculatureCircuitBuilder.h>
#include <brayns/circuit/api/reports/ReportFactory.h>
#include <brayns/circuit/api/reports/indexers/OffsetIndexer.h>

#include <brayns/circuit/components/RadiiReportData.h>
#include <brayns/circuit/components/ReportData.h>

#include <brayns/circuit/io/sonataloader/data/Config.h>
#include <brayns/circuit/io/sonataloader/data/Names.h>
#include <brayns/circuit/io/sonataloader/data/SimulationMapping.h>
#include <brayns/circuit/io/sonataloader/data/Vasculature.h>
#include <brayns/circuit/io/sonataloader/reports/SonataReportData.h>

#include <brayns/circuit/systems/RadiiReportSystem.h>

namespace
{
namespace sl = sonataloader;

class VasculatureFactory
{
public:
    static void create(sl::NodeLoadContext &context)
    {
        auto &population = context.population;
        auto &selection = context.selection;
        auto &params = context.params;
        auto &vasculatureParams = params.vasculature_geometry_parameters;
        auto multiplier = vasculatureParams.radius_multiplier;

        auto ids = selection.flatten();
        auto startPoints = sl::Vasculature::getSegmentStartPoints(population, selection);
        auto endPoints = sl::Vasculature::getSegmentEndPoints(population, selection);
        auto startRadii = sl::Vasculature::getSegmentStartRadii(population, selection);
        auto endRadii = sl::Vasculature::getSegmentEndRadii(population, selection);
        auto sections = sl::Vasculature::getSegmentSectionTypes(population, selection);

        _premultiplyRadii(multiplier, startRadii, endRadii);

        auto buildContext = VasculatureCircuitBuilder::Context{
            std::move(ids),
            std::move(startPoints),
            std::move(startRadii),
            std::move(endPoints),
            std::move(endRadii),
            std::move(sections)};
        VasculatureCircuitBuilder::build(context.model, std::move(buildContext));
    }

private:
    static void _premultiplyRadii(float multiplier, std::vector<float> &start, std::vector<float> &end)
    {
        if (multiplier == 1.f)
        {
            return;
        }

#pragma omp parallel for
        for (size_t i = 0; i < start.size(); ++i)
        {
            start[i] *= multiplier;
            end[i] *= multiplier;
        }
    }
};

class VasculatureReportFactory
{
public:
    static void create(sl::NodeLoadContext &context)
    {
        auto &cb = context.progress;
        cb.update();

        auto &params = context.params;
        auto reportType = params.report_type;
        if (reportType == sl::ReportType::None)
        {
            return;
        }
        if (reportType == sl::ReportType::BloodflowRadii)
        {
            _importRadiiReport(context);
            return;
        }
        _importReport(context);
    }

private:
    static std::string _getReportPath(sl::NodeLoadContext &context)
    {
        auto &params = context.params;
        auto reportName = params.report_name;
        auto &config = context.config;
        return config.getReportPath(reportName);
    }

    static std::vector<size_t> _getOffsets(sl::NodeLoadContext &context)
    {
        auto path = _getReportPath(context);
        auto &population = context.params.node_population;
        auto &selection = context.selection;
        auto flatSelection = selection.flatten();
        auto rawMapping = sl::SimulationMapping::getCompartmentMapping(path, population, flatSelection);

        auto sortedCompartments = std::map<uint64_t, size_t>();
        for (size_t i = 0; i < rawMapping.size(); ++i)
        {
            sortedCompartments[rawMapping[i][0]] = i;
        }

        auto offsets = std::vector<size_t>(sortedCompartments.size());
        auto it = sortedCompartments.begin();
        size_t index = 0;
        for (; it != sortedCompartments.end(); ++it, ++index)
        {
            offsets[index] = it->second;
        }
        return offsets;
    }

    static std::vector<float> _computeOriginalRadiiVector(brayns::Components &components)
    {
        auto &geometries = components.get<brayns::Geometries>();
        auto &geometry = geometries.elements.front();
        auto &capsules = *geometry.as<brayns::Capsule>();

        auto originalRadii = std::vector<float>();
        originalRadii.reserve(capsules.size());

        for (auto &capsule : capsules)
        {
            originalRadii.push_back(capsule.r0);
            originalRadii.push_back(capsule.r1);
        }

        return originalRadii;
    }

    static std::unique_ptr<IReportData> _createReportData(sl::NodeLoadContext &context)
    {
        auto path = _getReportPath(context);
        auto &population = context.params.node_population;
        auto &selection = context.selection;
        auto flatSelection = selection.flatten();
        return std::make_unique<sl::SonataReportData>(path, population, selection);
    }

    static void _importRadiiReport(sl::NodeLoadContext &context)
    {
        auto &model = context.model;
        auto &components = model.getComponents();
        auto originalRadii = _computeOriginalRadiiVector(components);
        auto offsets = _getOffsets(context);
        auto data = _createReportData(context);
        components.add<RadiiReportData>(std::move(data), std::move(offsets), std::move(originalRadii));
        auto &systems = model.getSystems();
        systems.setUpdateSystem<RadiiReportSystem>();
    }

    static void _importReport(sl::NodeLoadContext &context)
    {
        auto &model = context.model;
        auto data = _createReportData(context);
        auto offsets = _getOffsets(context);
        auto indexer = std::make_unique<OffsetIndexer>(std::move(offsets));
        auto reportData = ReportData{std::move(data), std::move(indexer)};
        ReportFactory::create(model, std::move(reportData));
    }
};
}

namespace sonataloader
{
std::string_view VasculaturePopulationLoader::getPopulationType() const noexcept
{
    return NodeNames::vasculature;
}

void VasculaturePopulationLoader::load(NodeLoadContext &context) const
{
    VasculatureFactory::create(context);
    VasculatureReportFactory::create(context);
}
} // namespace sonataloader
