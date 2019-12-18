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

#include <brayns/common/types.h>

struct TestRequest
{
    std::string message;
};
bool from_json(TestRequest& request, const std::string& payload);

struct TestResponse
{
    std::string answer;
};
std::string to_json(const TestResponse& response);

// Request a list of cell GIDs from a list of reports
struct CellGIDListRequest
{
    std::string path;
    strings targets;
};
bool from_json(CellGIDListRequest& request, const std::string& payload);

struct ModelCellGIDListRequest
{
    uint32_t modelId;
};
bool from_json(ModelCellGIDListRequest& request, const std::string& payload);

struct CellGIDList
{
    uint32_ts ids;
    int error;
    std::string message;
};
std::string to_json(const CellGIDList& list);

// Request a list of reports contained in a given circuit config file
struct ReportListRequest
{
    std::string path;
};
bool from_json(ReportListRequest& request, const std::string& payload);

struct ReportList
{
    strings reports;
    int error;
    std::string message;
};
std::string to_json(const ReportList& list);

// Request a list of targets contained in a given circuit config file
struct TargetListRequest
{
    std::string path;
};
bool from_json(TargetListRequest& request, const std::string& payload);

struct TargetList
{
    strings targets;
    int error;
    std::string message;
};
std::string to_json(const TargetList& list);

// Request a list of afferent cells from a list of source synapses cells from inside the circuit
struct AfferentGIDListRequest
{
    std::string path;
    uint32_ts sources;
};
bool from_json(AfferentGIDListRequest& request, const std::string& payload);

struct AfferentGIDList
{
    uint32_ts ids;
    int error;
    std::string message;
};
std::string to_json(const AfferentGIDList& list);

// Request a list of efferent cells from a list of source synapses cells from inside the circuit
struct EfferentGIDListRequest
{
    std::string path;
    uint32_ts sources;
};
bool from_json(EfferentGIDListRequest& request, const std::string& payload);

struct EfferentGIDList
{
    uint32_ts ids;
    int error;
    std::string message;
};
std::string to_json(const EfferentGIDList& list);

// Request a list of projections present on a given circuit configuration file
struct ProjectionListRequest
{
    std::string path;
};
bool from_json(ProjectionListRequest& request, const std::string& payload);

struct ProjectionList
{
    strings projections;
    int error;
    std::string message;
};
std::string to_json(const ProjectionList& list);

// Request the list of afferent cells from a list of source synapses cells from the given projection
struct ProjectionAfferentGIDListRequest
{
    std::string path;
    std::string projection;
    uint32_ts sources;
};
bool from_json(ProjectionAfferentGIDListRequest& request, const std::string& payload);

struct ProjectionAfferentGIDList
{
    uint32_ts ids;
    int error;
    std::string message;
};
std::string to_json(const ProjectionAfferentGIDList& list);

// Request the list of efferent cells from a list of source synapses cells from the given projection
struct ProjectionEfferentGIDListRequest
{
    std::string path;
    std::string projection;
    uint32_ts sources;
};
bool from_json(ProjectionEfferentGIDListRequest& request, const std::string& payload);

struct ProjectionEfferentGIDList
{
    uint32_ts ids;
    int error;
    std::string message;
};
std::string to_json(const ProjectionEfferentGIDList& list);

#endif
