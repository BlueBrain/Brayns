/* Copyright (c) 2019, EPFL/Blue Brain Project
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

#ifndef CIRCUITINFOPARAMS_H
#define CIRCUITINFOPARAMS_H

#include <brayns/network/message/ActionMessage.h>
#include <brayns/common/types.h>

struct CircuitInfoRequest : public brayns::Message
{
    MESSAGE_BEGIN(CircuitInfoRequest)
    MESSAGE_ENTRY(std::string, path, "Path to an existing circuit configuration file")
};

struct CircuitInfo : public brayns::Message
{
    MESSAGE_BEGIN(CircuitInfo)
    MESSAGE_ENTRY(uint64_t, cellsCount, "Number of cells in this circuit")
    MESSAGE_ENTRY(std::vector<std::string>, cellsProperties, "List of available cell properties")
    MESSAGE_ENTRY(std::vector<std::string>, mTypes, "List of morphology types available in this circuit")
    MESSAGE_ENTRY(std::vector<std::string>, eTypes, "List of electrical types available in this circuit")
    MESSAGE_ENTRY(std::vector<std::string>, targets, "List of target names")
    MESSAGE_ENTRY(std::vector<std::string>, reports, "List of report names")
    MESSAGE_ENTRY(std::string, spikeReport, "Path to the spike report file")
};

struct CellDataRequest : public brayns::Message
{
    MESSAGE_BEGIN(CellDataRequest)
    MESSAGE_ENTRY(std::string, path, "Path to an existing circuit configuration file")
    MESSAGE_ENTRY(std::vector<uint64_t>, ids, "List of cell IDs")
    MESSAGE_ENTRY(std::vector<std::string>, properties, "List of wanted properties")
};

struct CellData : public brayns::Message
{
    MESSAGE_BEGIN(CellData)
    MESSAGE_ENTRY(std::vector<std::string>, etypes, "Requested cell e-types")
    MESSAGE_ENTRY(std::vector<std::string>, mtypes, "Requested cell m-types")
    MESSAGE_ENTRY(std::vector<std::string>, morphologyClasses, "Requested cell morphology classes")
    MESSAGE_ENTRY(std::vector<std::string>, layers, "Requested cell layers")
    MESSAGE_ENTRY(std::vector<double>, positions, "Requested cell positions")
    MESSAGE_ENTRY(std::vector<double>, orientations, "Requested cell orientations")
};

struct CellGIDListRequest : public brayns::Message
{
    MESSAGE_BEGIN(CellGIDListRequest)
    MESSAGE_ENTRY(std::string, path, "Path to the circuit configuration file")
    MESSAGE_ENTRY(std::vector<std::string>, targets, "List of targets to query")
};

struct ModelCellGIDListRequest : public brayns::Message
{
    MESSAGE_BEGIN(ModelCellGIDListRequest)
    MESSAGE_ENTRY(uint32_t, modelId, "The ID of the model to query for cell GIDs")
};

struct CellGIDList : public brayns::Message
{
    MESSAGE_BEGIN(CellGIDList)
    MESSAGE_ENTRY(std::vector<unsigned>, ids, "List of cell GIDs")
};

struct ReportListRequest : public brayns::Message
{
    MESSAGE_BEGIN(ReportListRequest)
    MESSAGE_ENTRY(std::string, path, "Path to the circuit config to query for reports")
};

struct ReportList : public brayns::Message
{
    MESSAGE_BEGIN(ReportList)
    MESSAGE_ENTRY(std::vector<std::string>, reports, "List of report names")
};

struct ReportInfoRequest : public brayns::Message
{
    MESSAGE_BEGIN(ReportInfoRequest)
    MESSAGE_ENTRY(std::string, path, "Path to the circuit BlueConfig from which to get the report")
    MESSAGE_ENTRY(std::string, report, "Name of the report from where to get the information")
};

struct ReportInfo : public brayns::Message
{
    MESSAGE_BEGIN(ReportInfo)
    MESSAGE_ENTRY(double, startTime, "Time at which the simulation starts")
    MESSAGE_ENTRY(double, endTime, "Time at which the simulation ends")
    MESSAGE_ENTRY(double, timeStep, "Time between two consecutive simulation frames")
    MESSAGE_ENTRY(std::string, dataUnit, "Unit of the report values. Can be \"mV\", \"mA\", ...")
    MESSAGE_ENTRY(std::string, timeUnit, "Unit of the report time values")
    MESSAGE_ENTRY(uint64_t, frameCount, "Number of simulation frames in the report")
    MESSAGE_ENTRY(uint64_t, frameSize, "Number of values per frame in the report")
};

struct SpikeReportRequest : public brayns::Message
{
    MESSAGE_BEGIN(SpikeReportRequest)
    MESSAGE_ENTRY(std::string, path, "Path to the circuit to test");
};

struct SpikeReportInfo : public brayns::Message
{
    MESSAGE_BEGIN(SpikeReportInfo)
    MESSAGE_ENTRY(std::string, path, "Path to the spike report")
    MESSAGE_ENTRY(int32_t, exists, "Flag indicating wether there is a spike report")
};

struct TargetListRequest : public brayns::Message
{
    MESSAGE_BEGIN(TargetListRequest)
    MESSAGE_ENTRY(std::string, path, "Path to the circuit config to query for targets")
};

struct TargetList : public brayns::Message
{
    MESSAGE_BEGIN(TargetList)
    MESSAGE_ENTRY(std::vector<std::string>, targets, "List of target names")
};

struct AfferentGIDListRequest : public brayns::Message
{
    MESSAGE_BEGIN(AfferentGIDListRequest)
    MESSAGE_ENTRY(std::string, path, "Path to the circuit config to query for afferent synapses")
    MESSAGE_ENTRY(std::vector<unsigned>, sources, "List of synapse source cell GIDs")
};

struct AfferentGIDList : public brayns::Message
{
    MESSAGE_BEGIN(AfferentGIDList)
    MESSAGE_ENTRY(std::vector<unsigned>, ids, "List of afferent synapses cell GIDs")
};

struct EfferentGIDListRequest : public brayns::Message
{
    MESSAGE_BEGIN(EfferentGIDListRequest)
    MESSAGE_ENTRY(std::string, path, "Path to the circuit config to query for efferent synapses")
    MESSAGE_ENTRY(std::vector<unsigned>, sources, "List of synapse target cell GIDs")
};

struct EfferentGIDList : public brayns::Message
{
    MESSAGE_BEGIN(EfferentGIDList)
    MESSAGE_ENTRY(std::vector<unsigned>, ids, "List of efferent synapses cell GIDs")
};

struct ProjectionListRequest : public brayns::Message
{
    MESSAGE_BEGIN(ProjectionListRequest)
    MESSAGE_ENTRY(std::string, path, "Path to the circuit config to query for projection names")
};

struct ProjectionList : public brayns::Message
{
    MESSAGE_BEGIN(ProjectionList)
    MESSAGE_ENTRY(std::vector<std::string>, projections, "List of projection names")
};

struct ProjectionAfferentGIDListRequest : public brayns::Message
{
    MESSAGE_BEGIN(ProjectionAfferentGIDListRequest)
    MESSAGE_ENTRY(std::string, path, "Path to the circuit config to query for projected afferent"
                                     " synapses")
    MESSAGE_ENTRY(std::string, projection, "Projection name to query for")
    MESSAGE_ENTRY(std::vector<unsigned>, sources, "Projected source cell GIDs")
};

struct ProjectionAfferentGIDList : public brayns::Message
{
    MESSAGE_BEGIN(ProjectionAfferentGIDList)
    MESSAGE_ENTRY(std::vector<unsigned>, ids, "List of projected afferent synapses cell GIDs")
};

struct ProjectionEfferentGIDListRequest : public brayns::Message
{
    MESSAGE_BEGIN(ProjectionEfferentGIDListRequest)
    MESSAGE_ENTRY(std::string, path, "Path to the circuit config to query for projected efferent"
                                     " synapses")
    MESSAGE_ENTRY(std::string, projection, "Projection name to query for")
    MESSAGE_ENTRY(std::vector<unsigned>, sources, "Projected target cell GIDs")
};

struct ProjectionEfferentGIDList : public brayns::Message
{
    MESSAGE_BEGIN(ProjectionEfferentGIDList)
    MESSAGE_ENTRY(std::vector<unsigned>, ids, "List of projected efferent synapses cell GIDs")
};

#endif
