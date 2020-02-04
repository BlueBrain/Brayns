/* Copyright (c) 2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include "DTIParams.h"
#include "json.hpp"

#define FROM_JSON(PARAM, JSON, NAME) \
    PARAM.NAME = JSON[#NAME].get<decltype(PARAM.NAME)>()

bool from_json(StreamlinesDescriptor &param, const std::string &payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(param, js, name);
        FROM_JSON(param, js, gids);
        FROM_JSON(param, js, indices);
        FROM_JSON(param, js, vertices);
        FROM_JSON(param, js, radius);
        FROM_JSON(param, js, opacity);
        FROM_JSON(param, js, colorScheme);
        FROM_JSON(param, js, startModelId);
        FROM_JSON(param, js, endModelId);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool from_json(SpikeSimulationDescriptor &param, const std::string &payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(param, js, modelId);
        FROM_JSON(param, js, gids);
        FROM_JSON(param, js, timestamps);
        FROM_JSON(param, js, dt);
        FROM_JSON(param, js, endTime);
        FROM_JSON(param, js, timeScale);
        FROM_JSON(param, js, decaySpeed);
        FROM_JSON(param, js, restIntensity);
        FROM_JSON(param, js, spikeIntensity);
    }
    catch (...)
    {
        return false;
    }
    return true;
}
