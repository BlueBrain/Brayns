/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
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

#pragma once

#include <entrypoints/CIGetAfferentCellIdsEntrypoint.h>
#include <entrypoints/CIGetCellDataEntrypoint.h>
#include <entrypoints/CIGetCellIdsEntrypoint.h>
#include <entrypoints/CIGetCellIdsFromModelEntrypoint.h>
#include <entrypoints/CIGetEfferentCellIdsEntrypoint.h>
#include <entrypoints/CIGetProjectionEfferentCellIdsEntrypoint.h>
#include <entrypoints/CIGetProjectionsEntrypoint.h>
#include <entrypoints/CIGetReportInfoEntrypoint.h>
#include <entrypoints/CIGetReportsEntrypoint.h>
#include <entrypoints/CIGetSpikeReportInfoEntrypoint.h>
#include <entrypoints/CIGetTargetsEntrypoint.h>
#include <entrypoints/CIInfoEntrypoint.h>

#include "CircuitInfoPlugin.h"

class CircuitInfoEntrypoints
{
public:
    static void load(CircuitInfoPlugin& plugin)
    {
        plugin.add<CIInfoEntrypoint>();
        plugin.add<CIGetCellDataEntrypoint>();
        plugin.add<CIGetCellIdsEntrypoint>();
        plugin.add<CIGetCellIdsFromModelEntrypoint>();
        plugin.add<CIGetReportsEntrypoint>();
        plugin.add<CIGetReportInfoEntrypoint>();
        plugin.add<CIGetSpikeReportInfoEntrypoint>();
        plugin.add<CIGetTargetsEntrypoint>();
        plugin.add<CIGetAfferentCellIdsEntrypoint>();
        plugin.add<CIGetEfferentCellIdsEntrypoint>();
        plugin.add<CIGetProjectionsEntrypoint>();
        plugin.add<CIGetProjectionEfferentCellIdsEntrypoint>();
    }
};