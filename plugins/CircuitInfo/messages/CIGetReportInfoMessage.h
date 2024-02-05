/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include <brayns/json/JsonObjectMacro.h>

BRAYNS_JSON_OBJECT_BEGIN(CIGetReportInfoParams)
BRAYNS_JSON_OBJECT_ENTRY(
    std::string, path,
    "Path to the circuit BlueConfig from which to get the report")
BRAYNS_JSON_OBJECT_ENTRY(std::string, report,
                         "Name of the report from where to get the information")
BRAYNS_JSON_OBJECT_END()

BRAYNS_JSON_OBJECT_BEGIN(CIGetReportInfoResult)
BRAYNS_JSON_OBJECT_ENTRY(double, start_time,
                         "Time at which the simulation starts")
BRAYNS_JSON_OBJECT_ENTRY(double, end_time, "Time at which the simulation ends")
BRAYNS_JSON_OBJECT_ENTRY(double, time_step,
                         "Time between two consecutive simulation frames")
BRAYNS_JSON_OBJECT_ENTRY(std::string, data_unit,
                         "Unit of the report values. Can be (mV, mA, ...)")
BRAYNS_JSON_OBJECT_ENTRY(std::string, time_unit,
                         "Unit of the report time values")
BRAYNS_JSON_OBJECT_ENTRY(uint64_t, frame_count,
                         "Number of simulation frames in the report")
BRAYNS_JSON_OBJECT_ENTRY(uint64_t, frame_size,
                         "Number of values per frame in the report")
BRAYNS_JSON_OBJECT_END()
