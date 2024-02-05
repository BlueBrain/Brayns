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

BRAYNS_JSON_OBJECT_BEGIN(CIGetCellDataParams)
BRAYNS_JSON_OBJECT_ENTRY(std::string, path, "Path to circuit config file")
BRAYNS_JSON_OBJECT_ENTRY(std::vector<uint64_t>, ids, "List of cell IDs")
BRAYNS_JSON_OBJECT_ENTRY(std::vector<std::string>, properties,
                         "Desired properties")
BRAYNS_JSON_OBJECT_END()

BRAYNS_JSON_OBJECT_BEGIN(CIGetCellDataResult)
BRAYNS_JSON_OBJECT_ENTRY(std::vector<std::string>, etypes,
                         "Requested cell e-types")
BRAYNS_JSON_OBJECT_ENTRY(std::vector<std::string>, mtypes,
                         "Requested cell m-types")
BRAYNS_JSON_OBJECT_ENTRY(std::vector<std::string>, morphology_classes,
                         "Requested cell morphology classes")
BRAYNS_JSON_OBJECT_ENTRY(std::vector<std::string>, layers,
                         "Requested cell layers")
BRAYNS_JSON_OBJECT_ENTRY(std::vector<double>, positions,
                         "Requested cell positions")
BRAYNS_JSON_OBJECT_ENTRY(std::vector<double>, orientations,
                         "Requested cell orientations")
BRAYNS_JSON_OBJECT_END()
