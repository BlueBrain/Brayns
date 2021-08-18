/* Copyright (c) 2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrerou <nadir.romanguerrero@epfl.ch>
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

#include "CircuitInfoPlugin.h"
#include "Log.h"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>
#include <brayns/network/interface/ActionInterface.h>
#include <brayns/pluginapi/PluginAPI.h>

#include <brain/brain.h>
#include <brion/brion.h>

#include <set>

#include "CircuitInfoEntrypoints.h"

strings _splitString(const std::string& source, const char token)
{
    strings result;
    std::string split;
    std::istringstream ss(source);
    while (std::getline(ss, split, token))
        result.push_back(split);

    return result;
}

CircuitInfoPlugin::CircuitInfoPlugin()
    : ExtensionPlugin()
{
}

CircuitInfoPlugin::~CircuitInfoPlugin() {}

void CircuitInfoPlugin::init()
{
    auto actionInterface = _api->getActionInterface();

    if (!actionInterface)
    {
        return;
    }

    CircuitInfoEntrypoints::load(*actionInterface);

    if (false)
    {
        actionInterface->registerRequest<CircuitInfoRequest, CircuitInfo>(
            {"ci-info", "Return general info about a circuit",
             "CircuitInfoRequest",
             "Circuit from which to extract the requested information"},
            [&](const CircuitInfoRequest& request)
            { return _getCircuitInfo(request); });

        actionInterface->registerRequest<CellDataRequest, CellData>(
            {"ci-get-cell-data", "Return data attached to one or many cells",
             "CellDataRequest",
             "List of cells and properties that wants to be extracted"},
            [&](const CellDataRequest& request)
            { return _getCellData(request); });

        actionInterface->registerRequest<CellGIDListRequest, CellGIDList>(
            {"ci-get-cell-ids", "Return the list of GIDs from a circuit",
             "CellGIDListRequest",
             "Circuit from which to extract the GIDs, optionally from a list "
             "of targets"},
            [&](const CellGIDListRequest& request)
            { return _getCellGIDs(request); });

        actionInterface->registerRequest<ModelCellGIDListRequest, CellGIDList>(
            {"ci-get-cell-ids-from-model",
             "Return the list of GIDs from a loaded circuit",
             "ModelCellGIDListREquest",
             "The model from which to retrieve the GID list, optionally from a "
             "list of targets"},
            [&](const ModelCellGIDListRequest& request)
            { return _getCellGIDsFromModel(request); });

        actionInterface->registerRequest<ReportListRequest, ReportList>(
            {"ci-get-reports", "Return a list of reports from a circuit",
             "ReportListRequest",
             "The circuit from which to retrieve the list of reports"},
            [&](const ReportListRequest& request)
            { return _getReportList(request); });

        actionInterface->registerRequest<ReportInfoRequest, ReportInfo>(
            {"ci-get-report-info",
             "Return information about a specific report from a given circuit",
             "ReportInfoRequest",
             "The circuit and the report from which to get the information"},
            [&](const ReportInfoRequest& request)
            { return _getReportInfo(request); });

        actionInterface->registerRequest<SpikeReportRequest, SpikeReportInfo>(
            {"ci-get-spike-report-info",
             "Return wether the circuit has a spike report, and the path to it"
             " if exists",
             "SpikeReportRequest",
             "The circuit to which to query the spike report information"},
            [&](const SpikeReportRequest& request)
            { return _getSpikeReportInfo(request); });

        actionInterface->registerRequest<TargetListRequest, TargetList>(
            {"ci-get-targets", "Return a list of targets from a cricuit",
             "TargetListRequest",
             "The circuit from which to retrieve the list of targets"},
            [&](const TargetListRequest& request)
            { return _getTargetList(request); });

        actionInterface
            ->registerRequest<AfferentGIDListRequest, AfferentGIDList>(
                {"ci-get-afferent-cell-ids",
                 "Return a list of afferent synapses cell GIDs from a circuit "
                 "and a set of source "
                 "cells",
                 "AfferentGIDListRequest",
                 "The circuit and list of source cell GIDs from which to "
                 "return the afferent "
                 "syanpses cell GIDs"},
                [&](const AfferentGIDListRequest& request)
                { return _getAfferentGIDList(request); });

        actionInterface
            ->registerRequest<EfferentGIDListRequest, EfferentGIDList>(
                {"ci-get-efferent-cell-ids",
                 "Return a list of efferent synapses cell GIDs from a circuit "
                 "and set of source "
                 "cells",
                 "EfferentGIDListRequest",
                 "The circuit and list of source cell GIDs from which to "
                 "return the efferent "
                 "synapses cell GIDs"},
                [&](const EfferentGIDListRequest& request)
                { return _getEfferentGIDList(request); });

        actionInterface->registerRequest<ProjectionListRequest, ProjectionList>(
            {"ci-get-projections",
             "Return a list of projection names available on a circuit",
             "ProjectionListRequest",
             "The circuit from which to return the list of projection names"},
            [&](const ProjectionListRequest& request)
            { return _getProjectionList(request); });

        actionInterface->registerRequest<ProjectionEfferentGIDListRequest,
                                         ProjectionEfferentGIDList>(
            {"ci-get-projection-efferent-cell-ids",
             "Return a list of efferent projected synapses cell GIDs from a "
             "circuit and a set "
             "of source cells",
             "ProjectionEfferentGIDListRequest",
             "The circuit, the projection name and a set of source cells from "
             "which to return "
             "the list of projected efferent synapses cell GIDs"},
            [&](const ProjectionEfferentGIDListRequest& request)
            { return _getProjectionEfferentGIDList(request); });
    }
}

CircuitInfo CircuitInfoPlugin::_getCircuitInfo(
    const CircuitInfoRequest& request)
{
    CircuitInfo result;
    if (!boost::filesystem::exists(request.path))
    {
        result.setError(9, "Circuit not found");
        return result;
    }

    try
    {
        const brion::BlueConfig config(request.path);
        const brain::Circuit circuit(config);

        auto allGids = circuit.getGIDs();

        result.cellsCount = circuit.getNumNeurons();
        result.cellsProperties = {"etype", "mtype",    "morphology_class",
                                  "layer", "position", "orientation"};
        result.eTypes = circuit.getElectrophysiologyTypeNames();
        result.mTypes = circuit.getMorphologyTypeNames();
        result.reports = config.getSectionNames(brion::CONFIGSECTION_REPORT);
        result.spikeReport = config.getSpikeSource().getPath();

        const std::vector<brion::Target> targetParsers = config.getTargets();
        const std::vector<brion::TargetType> targetTypes = {
            brion::TargetType::TARGET_CELL,
            brion::TargetType::TARGET_COMPARTMENT,
            brion::TargetType::TARGET_ALL};
        for (const auto& t : targetParsers)
        {
            for (const auto& tt : targetTypes)
            {
                const brion::Strings& targetList = t.getTargetNames(tt);
                result.targets.insert(result.targets.end(), targetList.begin(),
                                      targetList.end());
            }
        }
    }
    catch (std::exception& e)
    {
        result.setError(2, e.what());
    }

    return result;
}

template <typename String,
          typename =
              std::enable_if<std::is_constructible<std::string, String>::value>>
inline bool propertyRequested(const CellDataRequest& request, String&& propName)
{
    auto it = std::find(request.properties.begin(), request.properties.end(),
                        propName);
    return it != request.properties.end();
}

CellData CircuitInfoPlugin::_getCellData(const CellDataRequest& request)
{
    CellData result;

    if (!boost::filesystem::exists(request.path))
    {
        result.setError(9, "Circuit not found");
        return result;
    }

    const brion::BlueConfig config(request.path);
    const brain::Circuit circuit(config);
    const brion::GIDSet gids(request.ids.begin(), request.ids.end());

    if (propertyRequested(request, "etype"))
    {
        auto etypeNames = circuit.getElectrophysiologyTypeNames();
        auto etypeIndices = circuit.getElectrophysiologyTypes(gids);
        result.etypes.reserve(etypeIndices.size());
        for (const auto index : etypeIndices)
            result.etypes.push_back(etypeNames[index]);
    }

    if (propertyRequested(request, "mtype"))
    {
        auto mtypeNames = circuit.getMorphologyTypeNames();
        auto mtypeIndices = circuit.getMorphologyTypes(gids);
        result.mtypes.reserve(mtypeIndices.size());
        for (const auto index : mtypeIndices)
            result.mtypes.push_back(mtypeNames[index]);
    }

    if (propertyRequested(request, "morphology_class"))
        result.morphologyClasses = circuit.getMorphologyNames(gids);

    if (propertyRequested(request, "layer"))
    {
        const auto& tsvFile =
            config.get(brion::BlueConfigSection::CONFIGSECTION_RUN, "Default",
                       "MEComboInfoFile");
        result.layers = circuit.getLayers(gids, tsvFile);
    }

    if (propertyRequested(request, "position"))
    {
        auto positions = circuit.getPositions(gids);
        result.positions.reserve(positions.size() * 3);
        for (const auto& pos : positions)
        {
            result.positions.push_back(pos.x);
            result.positions.push_back(pos.y);
            result.positions.push_back(pos.z);
        }
    }

    if (propertyRequested(request, "orientation"))
    {
        auto orientations = circuit.getRotations(gids);
        result.orientations.reserve(orientations.size() * 4);
        for (const auto& o : orientations)
        {
            result.orientations.push_back(o.w);
            result.orientations.push_back(o.x);
            result.orientations.push_back(o.y);
            result.orientations.push_back(o.z);
        }
    }

    return result;
}

CellGIDList CircuitInfoPlugin::_getCellGIDs(const CellGIDListRequest& request)
{
    CellGIDList result;

    if (!boost::filesystem::exists(request.path))
    {
        result.setError(9, "Circuit not found");
        return result;
    }

    try
    {
        const brion::BlueConfig config(request.path);
        const brain::Circuit circuit(config);

        brion::GIDSet gids;
        if (request.targets.empty())
            gids = circuit.getGIDs();
        else
        {
            for (const auto& target : request.targets)
            {
                const brion::GIDSet temp = circuit.getGIDs(target);
                gids.insert(temp.begin(), temp.end());
            }
        }
        result.ids.insert(result.ids.end(), gids.begin(), gids.end());
    }
    catch (std::exception& e)
    {
        result.setError(2, e.what());
    }

    return result;
}

CellGIDList CircuitInfoPlugin::_getCellGIDsFromModel(
    const ModelCellGIDListRequest& request)
{
    CellGIDList result;

    // Gather circuit path and targets from loaded model
    auto modelDescriptor =
        _api->getScene().getModel(static_cast<size_t>(request.modelId));
    if (modelDescriptor)
    {
        CellGIDListRequest castRequest;
        castRequest.path = modelDescriptor->getPath();

        // Gather all targets from the loaded model metadata
        const brayns::ModelMetadata& metaData = modelDescriptor->getMetadata();
        auto targetsIt = metaData.find("Targets");
        if (targetsIt != metaData.end())
        {
            const std::string& targetsString = targetsIt->second;
            std::vector<std::string> targets;
            if (!targetsString.empty())
            {
                if (targetsString.find(',') == std::string::npos)
                    castRequest.targets.push_back(targetsString);
                else
                {
                    castRequest.targets = _splitString(targetsString, ',');
                }
            }
        }

        // Return list of ids
        result = _getCellGIDs(castRequest);
    }
    else
    {
        result.setError(9, "Model not found");
    }

    return result;
}

ReportList CircuitInfoPlugin::_getReportList(const ReportListRequest& request)
{
    ReportList result;

    if (!boost::filesystem::exists(request.path))
    {
        result.setError(9, "Circuit not found");
        return result;
    }

    // Reports can be obtained as the names of the "report" section from the
    // BlueConfig file
    try
    {
        const brion::BlueConfig config(request.path);
        result.reports = config.getSectionNames(brion::CONFIGSECTION_REPORT);
    }
    catch (std::exception& e)
    {
        result.setError(2, e.what());
    }

    return result;
}

ReportInfo CircuitInfoPlugin::_getReportInfo(const ReportInfoRequest& request)
{
    ReportInfo result;

    if (!boost::filesystem::exists(request.path))
    {
        result.setError(9, "Circuit not found");
        return result;
    }

    // Reports can be obtained as the names of the "report" section from the
    // BlueConfig file
    try
    {
        const brion::BlueConfig config(request.path);
        const auto& reports =
            config.getSectionNames(brion::CONFIGSECTION_REPORT);
        auto it = std::find(reports.begin(), reports.end(), request.report);
        if (it == reports.end())
        {
            result.setError(1, "Report not found");
            return result;
        }

        const brion::URI reportPath = config.getReportSource(request.report);
        const brion::CompartmentReport report(reportPath,
                                              brion::AccessMode::MODE_READ);
        result.startTime = report.getStartTime();
        result.endTime = report.getEndTime();
        result.timeStep = report.getTimestep();
        result.dataUnit = report.getDataUnit();
        result.timeUnit = report.getTimeUnit();
        result.frameCount = report.getFrameCount();
        result.frameSize = report.getFrameSize();
    }
    catch (std::exception& e)
    {
        result.setError(2, e.what());
    }

    return result;
}

SpikeReportInfo CircuitInfoPlugin::_getSpikeReportInfo(
    const SpikeReportRequest& request)
{
    SpikeReportInfo result;

    if (!boost::filesystem::exists(request.path))
    {
        result.setError(9, "Circuit not found");
        return result;
    }

    // Reports can be obtained as the names of the "report" section from the
    // BlueConfig file
    try
    {
        const brion::BlueConfig config(request.path);
        result.path = config.getSpikeSource().getPath();
        result.exists =
            !result.path.empty() && boost::filesystem::exists(result.path);
    }
    catch (const std::exception& e)
    {
        result.setError(2, e.what());
    }

    return result;
}

TargetList CircuitInfoPlugin::_getTargetList(const TargetListRequest& request)
{
    TargetList result;

    if (!boost::filesystem::exists(request.path))
    {
        result.setError(9, "Circuit not found");
        return result;
    }

    // Gather all the targets for each type of target available
    try
    {
        const brion::BlueConfig config(request.path);
        const std::vector<brion::Target> targetParsers = config.getTargets();
        const std::vector<brion::TargetType> targetTypes = {
            brion::TargetType::TARGET_CELL,
            brion::TargetType::TARGET_COMPARTMENT,
            brion::TargetType::TARGET_ALL};
        for (const brion::Target& t : targetParsers)
        {
            for (const auto& tt : targetTypes)
            {
                const brion::Strings& targetList = t.getTargetNames(tt);
                result.targets.insert(result.targets.end(), targetList.begin(),
                                      targetList.end());
            }
        }
    }
    catch (std::exception& e)
    {
        result.setError(2, e.what());
    }

    return result;
}

AfferentGIDList CircuitInfoPlugin::_getAfferentGIDList(
    const AfferentGIDListRequest& request)
{
    AfferentGIDList result;

    if (!boost::filesystem::exists(request.path))
    {
        result.setError(9, "Circuit not found");
    }
    else
    {
        try
        {
            const brion::BlueConfig config(request.path);
            const brain::Circuit circuit(config);
            const brion::GIDSet uniqueSources(request.sources.begin(),
                                              request.sources.end());
            brain::SynapsesStream ss =
                circuit.getAfferentSynapses(uniqueSources);
            std::future<brain::Synapses> future = ss.read(ss.getRemaining());
            future.wait();
            const brain::Synapses afferentGIDs = future.get();
            const uint32_t* pgids = afferentGIDs.preGIDs();
            // Filter to have only unique ids
            const std::set<uint32_t> uniqueGIDs(pgids,
                                                pgids + afferentGIDs.size());
            result.ids.insert(result.ids.end(), uniqueGIDs.begin(),
                              uniqueGIDs.end());
        }
        catch (std::exception& e)
        {
            result.setError(2, std::string(e.what()));
        }
    }

    return result;
}

EfferentGIDList CircuitInfoPlugin::_getEfferentGIDList(
    const EfferentGIDListRequest& request)
{
    EfferentGIDList result;

    if (!boost::filesystem::exists(request.path))
    {
        result.setError(9, "Circuit not found");
    }
    else
    {
        try
        {
            const brion::BlueConfig config(request.path);
            const brain::Circuit circuit(config);
            const brion::GIDSet uniqueSources(request.sources.begin(),
                                              request.sources.end());
            brain::SynapsesStream ss =
                circuit.getEfferentSynapses(uniqueSources);
            std::future<brain::Synapses> future = ss.read(ss.getRemaining());
            future.wait();
            const brain::Synapses efferentGIDs = future.get();
            const uint32_t* pgids = efferentGIDs.postGIDs();
            // Filter to have only unique ids
            const std::set<uint32_t> uniqueGIDs(pgids,
                                                pgids + efferentGIDs.size());
            result.ids.insert(result.ids.end(), uniqueGIDs.begin(),
                              uniqueGIDs.end());
        }
        catch (std::exception& e)
        {
            result.setError(2, std::string(e.what()));
        }
    }

    return result;
}

ProjectionList CircuitInfoPlugin::_getProjectionList(
    const ProjectionListRequest& request)
{
    ProjectionList result;

    if (!boost::filesystem::exists(request.path))
    {
        result.setError(9, "Circuit not found");
        return result;
    }

    try
    {
        const brion::BlueConfig config(request.path);
        result.projections =
            config.getSectionNames(brion::CONFIGSECTION_PROJECTION);
    }
    catch (std::exception& e)
    {
        result.setError(2, std::string(e.what()));
    }

    return result;
}

ProjectionAfferentGIDList CircuitInfoPlugin::_getProjectionAfferentGIDList(
    const ProjectionAfferentGIDListRequest& request)
{
    ProjectionAfferentGIDList result;

    if (!boost::filesystem::exists(request.path))
    {
        result.setError(9, "Circuit not found");
        return result;
    }

    try
    {
        const brion::BlueConfig config(request.path);
        const std::string nrnPath = config.get(brion::CONFIGSECTION_PROJECTION,
                                               request.projection, "Path");

        if (nrnPath.empty())
        {
            result.setError(1, "Projection not found");
            return result;
        }

        const brain::Circuit circuit(config);
        const brion::GIDSet uniqueSources(request.sources.begin(),
                                          request.sources.end());
        brain::SynapsesStream ss =
            circuit.getExternalAfferentSynapses(uniqueSources,
                                                request.projection);
        std::future<brain::Synapses> future = ss.read(ss.getRemaining());
        future.wait();
        const brain::Synapses afferentGIDs = future.get();
        const uint32_t* pgids = afferentGIDs.preGIDs();
        // Filter to have only unique ids
        const std::set<uint32_t> uniqueGIDs(pgids, pgids + afferentGIDs.size());
        result.ids.insert(result.ids.end(), uniqueGIDs.begin(),
                          uniqueGIDs.end());
    }
    catch (std::exception& e)
    {
        result.setError(2, std::string(e.what()));
    }

    return result;
}

ProjectionEfferentGIDList CircuitInfoPlugin::_getProjectionEfferentGIDList(
    const ProjectionEfferentGIDListRequest& request)
{
    ProjectionEfferentGIDList result;

    if (!boost::filesystem::exists(request.path))
    {
        result.setError(9, "Circuit not found");
        return result;
    }

    try
    {
        const brion::BlueConfig config(request.path);
        const std::string nrnPath = config.get(brion::CONFIGSECTION_PROJECTION,
                                               request.projection, "Path");

        if (nrnPath.empty())
        {
            result.setError(1, "Projection not found");
            return result;
        }

        const brain::Circuit circuit(config);
        const brion::GIDSet uniqueSources(request.sources.begin(),
                                          request.sources.end());
        const uint32_ts gids =
            circuit.getProjectedEfferentGIDs(uniqueSources, request.projection);
        const std::set<uint32_t> uniqueGIDs(gids.begin(), gids.end());
        result.ids.insert(result.ids.end(), uniqueGIDs.begin(),
                          uniqueGIDs.end());
    }
    catch (std::exception& e)
    {
        result.setError(2, e.what());
    }

    return result;
}

extern "C" brayns::ExtensionPlugin* brayns_plugin_create(int /*argc*/,
                                                         char** /*argv*/)
{
    PLUGIN_INFO << "Initializing circuit info plugin" << std::endl;
    return new CircuitInfoPlugin();
}
