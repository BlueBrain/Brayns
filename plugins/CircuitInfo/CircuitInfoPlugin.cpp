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

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <brayns/common/ActionInterface.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>
#include <brayns/pluginapi/PluginAPI.h>

#include <brain/brain.h>
#include <brion/brion.h>

#include <set>

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

CircuitInfoPlugin::~CircuitInfoPlugin()
{
}

void CircuitInfoPlugin::init()
{
    auto actionInterface = _api->getActionInterface();
    if(actionInterface)
    {
        PLUGIN_INFO << "Registering 'test-request' endpoint" << std::endl;
        actionInterface->registerRequest<TestRequest, TestResponse>(
            "test-request", [](const TestRequest& request){
            TestResponse resp;
            resp.answer = std::string(request.message.rbegin(), request.message.rend());
            return resp;
        });

        PLUGIN_INFO << "Registering 'ci-get-cell-ids' endpoint" << std::endl;
        actionInterface->registerRequest<CellGIDListRequest, CellGIDList>(
            "ci-get-cell-ids", [&](const CellGIDListRequest& request){
            return _getCellGIDs(request);
        });

        PLUGIN_INFO << "Registering 'ci-get-cell-ids-from-model' endpoint" << std::endl;
        actionInterface->registerRequest<ModelCellGIDListRequest, CellGIDList>(
            "ci-get-cell-ids-from-model", [&](const ModelCellGIDListRequest& request){
            return _getCellGIDsFromModel(request);
        });

        PLUGIN_INFO << "Registering 'ci-get-reports' endpoint" << std::endl;
        actionInterface->registerRequest<ReportListRequest, ReportList>(
            "ci-get-reports", [&](const ReportListRequest& request){
            return _getReportList(request);
        });

        PLUGIN_INFO << "Registering 'ci-get-targets' endpoint" << std::endl;
        actionInterface->registerRequest<TargetListRequest, TargetList>(
            "ci-get-targets", [&](const TargetListRequest& request){
            return _getTargetList(request);
        });

        PLUGIN_INFO << "Registering 'ci-get-afferent-cell-ids' endpoint" << std::endl;
        actionInterface->registerRequest<AfferentGIDListRequest, AfferentGIDList>(
            "ci-get-afferent-cell-ids", [&](const AfferentGIDListRequest& request){
            return _getAfferentGIDList(request);
        });

        PLUGIN_INFO << "Registering 'ci-get-efferent-cell-ids' endpoint" << std::endl;
        actionInterface->registerRequest<EfferentGIDListRequest, EfferentGIDList>(
            "ci-get-efferent-cell-ids", [&](const EfferentGIDListRequest& request){
            return _getEfferentGIDList(request);
        });

        PLUGIN_INFO << "Registering 'ci-get-projections' endpoint" << std::endl;
        actionInterface->registerRequest<ProjectionListRequest, ProjectionList>(
            "ci-get-projections", [&](const ProjectionListRequest& request){
            return _getProjectionList(request);
        });
/*
        PLUGIN_INFO << "Registering 'ci-get-projection-afferent-cell-ids' endpoint" << std::endl;
        actionInterface->registerRequest<ProjectionAfferentGIDListRequest, ProjectionAfferentGIDList>(
            "ci-get-projection-afferent-cell-ids", [&](const ProjectionAfferentGIDListRequest& request){
            return _getProjectionAfferentGIDList(request);
        });
*/
        PLUGIN_INFO << "Registering 'ci-get-projection-efferent-cell-ids' endpoint" << std::endl;
        actionInterface->registerRequest<ProjectionEfferentGIDListRequest, ProjectionEfferentGIDList>(
            "ci-get-projection-efferent-cell-ids", [&](const ProjectionEfferentGIDListRequest& request){
            return _getProjectionEfferentGIDList(request);
        });
    }
}

CellGIDList CircuitInfoPlugin::_getCellGIDs(const CellGIDListRequest& request)
{
    CellGIDList result;
    result.error = 0;
    result.message = "";

    if (!boost::filesystem::exists(request.path))
    {
        result.error = 9;
        result.message = "Circuit not found";
        return result;
    }

    const brion::BlueConfig config(request.path);
    const brain::Circuit circuit (config);

    try
    {
        brion::GIDSet gids;
        if(request.targets.empty())
            gids = circuit.getGIDs();
        else
        {
            for(const auto& target : request.targets)
            {
                const brion::GIDSet temp = circuit.getGIDs(target);
                gids.insert(temp.begin(), temp.end());
            }
        }
        result.ids = uint32_ts(gids.begin(), gids.end());
    }
    catch(...)
    {
        result.error = 2;
        result.message = "Unknown error while parsing circuit";
    }

    return result;
}

CellGIDList CircuitInfoPlugin::_getCellGIDsFromModel(const ModelCellGIDListRequest& request)
{
    CellGIDList result;
    result.error = 0;
    result.message = "";

    // Gather circuit path and targets from loaded model
    auto modelDescriptor = _api->getScene().getModel(static_cast<size_t>(request.modelId));
    if(modelDescriptor)
    {
        CellGIDListRequest castRequest;
        castRequest.path = modelDescriptor->getPath();

        // Gather all targets from the loaded model metadata
        const brayns::ModelMetadata& metaData = modelDescriptor->getMetadata();
        auto targetsIt = metaData.find("Targets");
        if(targetsIt != metaData.end())
        {
            const std::string& targetsString = targetsIt->second;
            if(!targetsString.empty())
            {
                if(targetsString.find(',') == std::string::npos)
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
        result.error = 9;
        result.message = "Model not found";
    }

    return result;
}

ReportList CircuitInfoPlugin::_getReportList(const ReportListRequest& request)
{
    ReportList result;
    result.error = 0;
    result.message = "";

    if(!boost::filesystem::exists(request.path))
    {
        result.error = 9;
        result.message = "Circuit not found";
        return result;
    }

    // Reports can be obtained as the names of the "report" section from the BlueConfig file
    const brion::BlueConfig config(request.path);
    result.reports = config.getSectionNames(brion::CONFIGSECTION_REPORT);

    return result;
}

TargetList CircuitInfoPlugin::_getTargetList(const TargetListRequest& request)
{
    TargetList result;
    result.error = 0;
    result.message = "";

    if(!boost::filesystem::exists(request.path))
    {
        result.error = 9;
        result.message = "Circuit not found";
        return result;
    }

    // Gather all the targets for each type of target available
    const brion::BlueConfig config(request.path);
    const std::vector<brion::Target> targetParsers =  config.getTargets();
    const std::vector<brion::TargetType> targetTypes = {brion::TargetType::TARGET_CELL,
                                                         brion::TargetType::TARGET_COMPARTMENT,
                                                         brion::TargetType::TARGET_ALL};
    for(const brion::Target& t : targetParsers)
    {
        for(const auto& tt : targetTypes)
        {
            const brion::Strings& targetList = t.getTargetNames(tt);
            result.targets.insert(result.targets.end(), targetList.begin(), targetList.end());
        }
    }

    return result;
}

AfferentGIDList CircuitInfoPlugin::_getAfferentGIDList(const AfferentGIDListRequest& request)
{
    AfferentGIDList result;
    result.error = 0;
    result.message = "";

    if(!boost::filesystem::exists(request.path))
    {
        result.error = 9;
        result.message = "Circuit not found";
    }
    else
    {
        const brion::BlueConfig config(request.path);
        const brain::Circuit circuit (config);
        const brion::GIDSet uniqueSources (request.sources.begin(), request.sources.end());
        brain::SynapsesStream ss = circuit.getAfferentSynapses(uniqueSources);
        std::future<brain::Synapses> future = ss.read(ss.getRemaining());
        future.wait();
        const brain::Synapses afferentGIDs = future.get();
        const uint32_t* pgids = afferentGIDs.preGIDs();
        // Filter to have only unique ids
        const std::set<uint32_t> uniqueGIDs (pgids, pgids + afferentGIDs.size());
        result.ids = uint32_ts(uniqueGIDs.begin(), uniqueGIDs.end());
    }

    return result;
}

EfferentGIDList CircuitInfoPlugin::_getEfferentGIDList(const EfferentGIDListRequest& request)
{
    EfferentGIDList result;
    result.error = 0;
    result.message = "";

    if(!boost::filesystem::exists(request.path))
    {
        result.error = 9;
        result.message = "Circuit not found";
    }
    else
    {
        const brion::BlueConfig config(request.path);
        const brain::Circuit circuit (config);
        const brion::GIDSet uniqueSources (request.sources.begin(), request.sources.end());
        brain::SynapsesStream ss = circuit.getEfferentSynapses(uniqueSources);
        std::future<brain::Synapses> future = ss.read(ss.getRemaining());
        future.wait();
        const brain::Synapses efferentGIDs = future.get();
        const uint32_t* pgids = efferentGIDs.postGIDs();
        // Filter to have only unique ids
        const std::set<uint32_t> uniqueGIDs (pgids, pgids + efferentGIDs.size());
        result.ids = uint32_ts(uniqueGIDs.begin(), uniqueGIDs.end());
    }

    return result;
}

ProjectionList CircuitInfoPlugin::_getProjectionList(const ProjectionListRequest& request)
{
    ProjectionList result;
    result.error = 0;
    result.message = "";

    if(!boost::filesystem::exists(request.path))
    {
        result.error = 9;
        result.message = "Circuit not found";
        return result;
    }

    const brion::BlueConfig config(request.path);
    result.projections = config.getSectionNames(brion::CONFIGSECTION_PROJECTION);

    return result;
}

ProjectionAfferentGIDList
CircuitInfoPlugin::_getProjectionAfferentGIDList(const ProjectionAfferentGIDListRequest& request)
{
    ProjectionAfferentGIDList result;
    result.error = 0;
    result.message = "";

    if(!boost::filesystem::exists(request.path))
    {
        result.error = 9;
        result.message = "Circuit not found";
        return result;
    }

    const brion::BlueConfig config(request.path);
    const std::string nrnPath = config.get(brion::CONFIGSECTION_PROJECTION, request.projection, "Path");

    if(nrnPath.empty())
    {
        result.error = 1;
        result.message = "Projection not found";
        return result;
    }

    const brain::Circuit circuit (config);
    const brion::GIDSet uniqueSources (request.sources.begin(), request.sources.end());
    brain::SynapsesStream ss = circuit.getExternalAfferentSynapses(uniqueSources, request.projection);
    std::future<brain::Synapses> future = ss.read(ss.getRemaining());
    future.wait();
    const brain::Synapses afferentGIDs = future.get();
    const uint32_t* pgids = afferentGIDs.preGIDs();
    // Filter to have only unique ids
    const std::set<uint32_t> uniqueGIDs (pgids, pgids + afferentGIDs.size());
    result.ids = uint32_ts(uniqueGIDs.begin(), uniqueGIDs.end());

    return result;
}

ProjectionEfferentGIDList
CircuitInfoPlugin::_getProjectionEfferentGIDList(const ProjectionEfferentGIDListRequest& request)
{
    ProjectionEfferentGIDList result;
    result.error = 0;
    result.message = "";

    if(!boost::filesystem::exists(request.path))
    {
        result.error = 9;
        result.message = "Circuit not found";
        return result;
    }

    const brion::BlueConfig config(request.path);
    const std::string nrnPath = config.get(brion::CONFIGSECTION_PROJECTION, request.projection, "Path");

    if(nrnPath.empty())
    {
        result.error = 1;
        result.message = "Projection not found";
        return result;
    }

    const brain::Circuit circuit (config);
    const brion::GIDSet uniqueSources (request.sources.begin(), request.sources.end());
    const uint32_ts gids = circuit.getProjectedEfferentGIDs(uniqueSources, request.projection);
    const std::set<uint32_t> uniqueGIDs (gids.begin(), gids.end());
    result.ids = uint32_ts(uniqueGIDs.begin(), uniqueGIDs.end());

    return result;
}

extern "C" brayns::ExtensionPlugin* brayns_plugin_create(int /*argc*/,
                                                         char** /*argv*/)
{
    PLUGIN_INFO << "Initializing circuit info plugin" << std::endl;
    return new CircuitInfoPlugin();
}
