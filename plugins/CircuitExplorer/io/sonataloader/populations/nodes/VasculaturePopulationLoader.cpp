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

#include <api/reports/ReportFactory.h>
#include <api/reports/indexers/OffsetIndexer.h>
#include <api/vasculature/VasculatureColorHandler.h>
#include <components/CircuitIds.h>
#include <components/ColorList.h>
#include <components/Coloring.h>
#include <components/RadiiReportData.h>
#include <components/ReportData.h>
#include <components/VasculatureSectionList.h>
#include <io/sonataloader/colordata/node/VasculatureColorData.h>
#include <io/sonataloader/data/Config.h>
#include <io/sonataloader/data/SonataNames.h>
#include <io/sonataloader/data/SonataSimulationMapping.h>
#include <io/sonataloader/data/SonataVasculature.h>
#include <io/sonataloader/populations/nodes/common/ColorDataFactory.h>
#include <io/sonataloader/reports/SonataReportData.h>
#include <systems/RadiiReportSystem.h>

namespace
{
namespace sl = sonataloader;

class VasculaturePrimitiveImporter
{
public:
    static std::vector<brayns::Capsule> import(sl::NodeLoadContext &context)
    {
        auto &cb = context.progress;
        auto &population = context.population;
        auto &selection = context.selection;
        auto &params = context.params;
        auto &vasculatureParams = params.vasculature_geometry_parameters;
        auto multiplier = vasculatureParams.radius_multiplier;

        auto startPoints = sl::SonataVasculature::getSegmentStartPoints(population, selection);
        auto endPoints = sl::SonataVasculature::getSegmentEndPoints(population, selection);
        auto startRadii = sl::SonataVasculature::getSegmentStartRadii(population, selection);
        auto endRadii = sl::SonataVasculature::getSegmentEndRadii(population, selection);

        auto size = startPoints.size();
        auto primitives = std::vector<brayns::Capsule>();
        primitives.reserve(size);

        for (size_t i = 0; i < size; ++i)
        {
            cb.update("Loading vasculature geometry");
            auto &p0 = startPoints[i];
            auto r0 = startRadii[i] * multiplier;
            auto &p1 = endPoints[i];
            auto r1 = endRadii[i] * multiplier;
            primitives.push_back(brayns::CapsuleFactory::cone(p0, r0, p1, r1));
        }
        return primitives;
    }
};

class VasculatureSectionImporter
{
public:
    static std::vector<VasculatureSection> import(sl::NodeLoadContext &context)
    {
        auto &population = context.population;
        auto &selection = context.selection;
        return sl::SonataVasculature::getSegmentSectionTypes(population, selection);
    }
};

class ModelBuilder
{
public:
    ModelBuilder(brayns::Model &model)
        : _components(model.getComponents())
        , _systems(model.getSystems())
    {
    }

    void addGeometry(std::vector<brayns::Capsule> primitives)
    {
        auto &geometries = _components.add<brayns::Geometries>();
        geometries.elements.emplace_back(std::move(primitives));
    }

    void addColoring(std::unique_ptr<IColorData> colorData)
    {
        auto colorHandler = std::make_unique<VasculatureColorHandler>(_components);
        _components.add<Coloring>(std::move(colorData), std::move(colorHandler));
    }

    void addSections(std::vector<VasculatureSection> sections)
    {
        _components.add<VasculatureSectionList>(std::move(sections));
    }

    void addColorList(size_t numItems)
    {
        auto &colorList = _components.add<ColorList>();
        colorList.elements.resize(numItems, brayns::Vector4f(1.f));
    }

    void addIds(std::vector<uint64_t> ids)
    {
        _components.add<CircuitIds>(std::move(ids));
    }

    void addSystems()
    {
        _systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Geometries>>();
        _systems.setInitSystem<brayns::GeometryInitSystem>();
        _systems.setCommitSystem<brayns::GeometryCommitSystem>();
    }

private:
    brayns::Components &_components;
    brayns::Systems &_systems;
};

class VasculatureFactory
{
public:
    static void create(sl::NodeLoadContext &context)
    {
        auto primitives = VasculaturePrimitiveImporter::import(context);
        auto sections = VasculatureSectionImporter::import(context);
        auto ids = context.selection.flatten();
        auto colorData = sl::NodeColorDataFactory::create<sl::VasculatureColorData>(context);

        auto builder = ModelBuilder(context.model);
        builder.addGeometry(std::move(primitives));
        builder.addColorList(ids.size());
        builder.addIds(std::move(ids));
        builder.addColoring(std::move(colorData));
        builder.addSections(std::move(sections));
        builder.addSystems();
    }
};

class VasculatureReportFactory
{
public:
    static void create(sl::NodeLoadContext &context)
    {
        auto &cb = context.progress;
        cb.update("Loading vasculature report");

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
        auto rawMapping = sl::SonataSimulationMapping::getCompartmentMapping(path, population, flatSelection);

        auto sortedCompartments = std::map<uint64_t, size_t>();
        for (size_t i = 0; i < rawMapping.size(); ++i)
        {
            sortedCompartments[rawMapping[i].first] = i;
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

        std::vector<float> originalRadii;
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
    return SonataNodeNames::vasculature;
}

void VasculaturePopulationLoader::load(NodeLoadContext &context) const
{
    VasculatureFactory::create(context);
    VasculatureReportFactory::create(context);
}
} // namespace sonataloader
