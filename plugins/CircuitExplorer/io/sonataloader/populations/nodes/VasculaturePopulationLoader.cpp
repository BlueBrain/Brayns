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

#include <api/reports/indexers/OffsetIndexer.h>
#include <api/vasculature/VasculatureColorHandler.h>
#include <components/CircuitColorComponent.h>
#include <components/ReportComponent.h>
#include <components/VasculatureComponent.h>
#include <components/VasculatureRadiiReportComponent.h>
#include <io/sonataloader/colordata/node/VasculatureColorData.h>
#include <io/sonataloader/data/SonataConfig.h>
#include <io/sonataloader/data/SonataSimulationMapping.h>
#include <io/sonataloader/data/SonataVasculature.h>
#include <io/sonataloader/populations/nodes/common/ColorDataFactory.h>
#include <io/sonataloader/reports/SonataReportData.h>

namespace
{
namespace sl = sonataloader;

struct VasculatureImporter
{
    static void import(sl::NodeLoadContext &ctxt)
    {
        auto &cb = ctxt.progress;
        cb.update("Loading vasculature geometry");

        const auto &population = ctxt.population;
        const auto &selection = ctxt.selection;
        const auto &params = ctxt.params;
        const auto &vasculatureGeometrySettings = params.vasculature_geometry_parameters;
        const auto radiusMultiplier = vasculatureGeometrySettings.radius_multiplier;

        const auto startPoints = sl::SonataVasculature::getSegmentStartPoints(population, selection);
        const auto endPoints = sl::SonataVasculature::getSegmentEndPoints(population, selection);
        auto sections = sl::SonataVasculature::getSegmentSectionTypes(population, selection);

        auto startRadii = sl::SonataVasculature::getSegmentStartRadii(population, selection);
        auto endRadii = sl::SonataVasculature::getSegmentEndRadii(population, selection);
        if (radiusMultiplier != 1.f)
        {
            std::transform(
                startRadii.begin(),
                startRadii.end(),
                startRadii.begin(),
                [mult = radiusMultiplier](const float r) { return r * mult; });
            std::transform(
                endRadii.begin(),
                endRadii.end(),
                endRadii.begin(),
                [mult = radiusMultiplier](const float r) { return r * mult; });
        }

        auto ids = selection.flatten();
        auto geometry = std::vector<brayns::Primitive>(ids.size());

#pragma omp parallel for
        for (size_t i = 0; i < ids.size(); ++i)
        {
            const auto &p0 = startPoints[i];
            const auto r0 = startRadii[i];
            const auto &p1 = endPoints[i];
            const auto r1 = endRadii[i];

            geometry[i] = brayns::Primitive::cone(p0, r0, p1, r1);
        }

        auto &model = ctxt.model;
        auto &vasculature =
            model.addComponent<VasculatureComponent>(std::move(ids), std::move(geometry), std::move(sections));

        auto colorHandler = std::make_unique<VasculatureColorHandler>(vasculature);
        auto colorData = sl::NodeColorDataFactory::create<sl::VasculatureColorData>(ctxt);
        model.addComponent<CircuitColorComponent>(std::move(colorData), std::move(colorHandler));
    }
};

struct VasculatureReportImporter
{
    static void import(sl::NodeLoadContext &ctxt)
    {
        auto &cb = ctxt.progress;
        const auto &params = ctxt.params;
        const auto reportType = params.report_type;
        const auto reportName = params.report_name;
        const auto &network = ctxt.config;
        const auto &simConfig = network.simulationConfig();
        const auto path = sl::SonataConfig::resolveReportPath(simConfig, reportName);

        const auto &population = ctxt.population;
        const auto populationName = population.name();

        const auto &selection = ctxt.selection;
        const auto flatSelection = selection.flatten();

        const auto rawMapping = sl::SonataSimulationMapping::getCompartmentMapping(path, populationName, flatSelection);

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
        auto &model = ctxt.model;

        if (reportType == sl::ReportType::BLOODFLOW_RADII)
        {
            cb.update("Loading vasculature radii report");
            model.addComponent<VasculatureRadiiReportComponent>(std::move(data), std::move(offsets));
        }
        else
        {
            cb.update("Loading bloodflow report");
            auto indexer = std::make_unique<OffsetIndexer>(std::move(offsets));
            model.addComponent<ReportComponent>(std::move(data), std::move(indexer));
        }
    }
};
}

namespace sonataloader
{
std::string VasculaturePopulationLoader::getPopulationType() const noexcept
{
    return "vasculature";
}

void VasculaturePopulationLoader::load(NodeLoadContext &ctxt) const
{
    VasculatureImporter::import(ctxt);

    const auto &params = ctxt.params;
    const auto reportType = params.report_type;
    if (reportType != ReportType::NONE)
    {
        VasculatureReportImporter::import(ctxt);
    }
}
} // namespace sonataloader
