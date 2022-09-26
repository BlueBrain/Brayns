/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/geometry/types/Capsule.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>
#include <brayns/engine/systems/GeometryCommitSystem.h>
#include <brayns/engine/systems/GeometryInitSystem.h>

#include <api/reports/indexers/OffsetIndexer.h>
#include <api/vasculature/VasculatureColorHandler.h>
#include <components/CircuitIds.h>
#include <components/ColorList.h>
#include <components/Coloring.h>
#include <components/RadiiReportData.h>
#include <components/ReportData.h>
#include <components/VasculatureSectionList.h>
#include <io/sonataloader/colordata/node/VasculatureColorData.h>
#include <io/sonataloader/data/SonataConfig.h>
#include <io/sonataloader/data/SonataSimulationMapping.h>
#include <io/sonataloader/data/SonataVasculature.h>
#include <io/sonataloader/populations/nodes/common/ColorDataFactory.h>
#include <io/sonataloader/reports/SonataReportData.h>
#include <systems/RadiiReportSystem.h>
#include <systems/ReportSystem.h>

namespace
{
namespace sl = sonataloader;

class VasculatureImporter
{
public:
    static void import(sl::NodeLoadContext &context)
    {
        auto &cb = context.progress;
        cb.update("Loading vasculature geometry");

        auto &population = context.population;
        auto &selection = context.selection;
        auto &params = context.params;
        auto &vasculatureGeometrySettings = params.vasculature_geometry_parameters;
        auto radiusMultiplier = vasculatureGeometrySettings.radius_multiplier;

        auto startPoints = sl::SonataVasculature::getSegmentStartPoints(population, selection);
        auto endPoints = sl::SonataVasculature::getSegmentEndPoints(population, selection);
        auto sections = sl::SonataVasculature::getSegmentSectionTypes(population, selection);
        auto startRadii = sl::SonataVasculature::getSegmentStartRadii(population, selection);
        auto endRadii = sl::SonataVasculature::getSegmentEndRadii(population, selection);

        _applyRadiusMultiplier(startRadii, endRadii, radiusMultiplier);

        auto ids = selection.flatten();
        auto geometry = std::vector<brayns::Capsule>(ids.size());

#pragma omp parallel for
        for (size_t i = 0; i < ids.size(); ++i)
        {
            auto &p0 = startPoints[i];
            auto r0 = startRadii[i];
            auto &p1 = endPoints[i];
            auto r1 = endRadii[i];
            geometry[i] = brayns::CapsuleFactory::cone(p0, r0, p1, r1);
        }

        auto &model = context.model;
        auto &components = model.getComponents();

        auto colorHandler = std::make_unique<VasculatureColorHandler>(components);
        auto colorData = sl::NodeColorDataFactory::create<sl::VasculatureColorData>(context);

        components.add<CircuitIds>(std::move(ids));
        auto &geometries = components.add<brayns::Geometries>();
        geometries.elements.emplace_back(std::move(geometry));
        components.add<VasculatureSectionList>(std::move(sections));
        components.add<Coloring>(std::move(colorData), std::move(colorHandler));
    }

private:
    static void _applyRadiusMultiplier(std::vector<float> &start, std::vector<float> &end, float multiplier)
    {
        if (multiplier == 1.f)
        {
            return;
        }
        for (size_t i = 0; i < start.size(); ++i)
        {
            start[i] *= multiplier;
            end[i] *= multiplier;
        }
    }
};

class VasculatureReportImporter
{
public:
    static void import(sl::NodeLoadContext &context)
    {
        auto &cb = context.progress;
        auto &params = context.params;
        auto reportType = params.report_type;
        auto reportName = params.report_name;
        auto &network = context.config;
        auto &simConfig = network.simulationConfig();
        auto path = sl::SonataConfig::resolveReportPath(simConfig, reportName);

        auto &population = context.population;
        auto populationName = population.name();

        auto &selection = context.selection;
        auto flatSelection = selection.flatten();

        auto rawMapping = sl::SonataSimulationMapping::getCompartmentMapping(path, populationName, flatSelection);

        // Sorted vasculature mapping indices
        std::map<uint64_t, size_t> sortedCompartmentsSize;
        for (size_t i = 0; i < rawMapping.size(); ++i)
        {
            sortedCompartmentsSize[rawMapping[i].first] = i;
        }

        // Flatten
        std::vector<size_t> offsets(sortedCompartmentsSize.size());
        auto it = sortedCompartmentsSize.begin();
        size_t index = 0;
        while (it != sortedCompartmentsSize.end())
        {
            offsets[index] = it->second;
            ++it;
            ++index;
        }

        auto data = std::make_unique<sl::SonataReportData>(path, populationName, selection);
        auto &model = context.model;

        if (reportType == sl::ReportType::BloodflowRadii)
        {
            cb.update("Loading vasculature radii report");
            auto &components = model.getComponents();

            auto originalRadii = _computeOriginalRadiiVector(components);

            components.add<RadiiReportData>(std::move(data), std::move(offsets), std::move(originalRadii));
            model.getSystems().setPreRenderSystem<RadiiReportSystem>();
            return;
        }

        cb.update("Loading bloodflow report");
        auto indexer = std::make_unique<OffsetIndexer>(std::move(offsets));
        model.getComponents().add<ReportData>(std::move(data), std::move(indexer));
        model.getSystems().setPreRenderSystem<ReportSystem>();
    }

private:
    static std::vector<float> _computeOriginalRadiiVector(brayns::Components &components)
    {
        auto &geometries = components.get<brayns::Geometries>();
        auto &geometry = geometries.elements.front();
        auto &capsules = *geometry.as<brayns::Capsule>();

        std::vector<float> originalRadii;
        originalRadii.reserve(capsules.size());
        for (auto &capsule : capsules)
        {
            originalRadii.push_back(capsule.r0);
            originalRadii.push_back(capsule.r1);
        }
        return originalRadii;
    }
};
}

namespace sonataloader
{
std::string VasculaturePopulationLoader::getPopulationType() const noexcept
{
    return "vasculature";
}

void VasculaturePopulationLoader::load(NodeLoadContext &context) const
{
    VasculatureImporter::import(context);

    auto &params = context.params;
    auto reportType = params.report_type;
    if (reportType != ReportType::None)
    {
        VasculatureReportImporter::import(context);
    }
}
} // namespace sonataloader
