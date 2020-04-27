/* Copyright (c) 2020, EPFL/Blue Brain Project
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
#include "PBRVolumesParams.h"
#include "Log.h"

#include "json.hpp"

#ifndef BRAYNS_DEBUG_JSON_ENABLED
#define FROM_JSON(PARAM, JSON, NAME) \
    PARAM.NAME = JSON[#NAME].get<decltype(PARAM.NAME)>()
#else
#define FROM_JSON(PARAM, JSON, NAME) \
    try { \
        PARAM.NAME = JSON[#NAME].get<decltype(PARAM.NAME)>(); \
    } \
    catch(...){ \
        PLUGIN_ERROR << "JSON parsing error for attribute '" << #NAME<< "'!" << std::endl; \
        throw; \
    }
#endif
#define TO_JSON(PARAM, JSON, NAME) JSON[#NAME] = PARAM.NAME


bool from_json(AddHomogeneusVolume& request, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(request, js, name);
        FROM_JSON(request, js, absorption);
        FROM_JSON(request, js, scattering);
        FROM_JSON(request, js, scale);
        FROM_JSON(request, js, g);
        FROM_JSON(request, js, Le);
        FROM_JSON(request, js, density);
        FROM_JSON(request, js, p0);
        FROM_JSON(request, js, p1);
    }
    catch (const std::exception& except)
    {
        request.parsed = false;
        request.parseMessage = std::string(except.what());
    }

    return true;
}


bool from_json(AddHomogeneusVolumeToModel& request, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(request, js, name);
        FROM_JSON(request, js, absorption);
        FROM_JSON(request, js, scattering);
        FROM_JSON(request, js, scale);
        FROM_JSON(request, js, g);
        FROM_JSON(request, js, Le);
        FROM_JSON(request, js, density);
        FROM_JSON(request, js, modelId);
    }
    catch (const std::exception& except)
    {
        request.parsed = false;
        request.parseMessage = std::string(except.what());
    }

    return true;
}


bool from_json(AddHeterogeneusVolume& request, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(request, js, name);
        FROM_JSON(request, js, absorption);
        FROM_JSON(request, js, scattering);
        FROM_JSON(request, js, scale);
        FROM_JSON(request, js, g);
        FROM_JSON(request, js, Le);
        FROM_JSON(request, js, minDensity);
        FROM_JSON(request, js, maxDensity);
        FROM_JSON(request, js, p0);
        FROM_JSON(request, js, p1);
    }
    catch (const std::exception& except)
    {
        request.parsed = false;
        request.parseMessage = std::string(except.what());
    }

    return true;
}


bool from_json(AddGridVolume& request, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(request, js, name);
        FROM_JSON(request, js, absorption);
        FROM_JSON(request, js, scattering);
        FROM_JSON(request, js, scale);
        FROM_JSON(request, js, g);
        FROM_JSON(request, js, Le);
        FROM_JSON(request, js, p0);
        FROM_JSON(request, js, p1);
        FROM_JSON(request, js, density);
        FROM_JSON(request, js, nx);
        FROM_JSON(request, js, ny);
        FROM_JSON(request, js, nz);
    }
    catch (const std::exception& except)
    {
        request.parsed = false;
        request.parseMessage = std::string(except.what());
    }

    return true;
}


std::string to_json(const AddVolumeResponse& response)
{
    try
    {
        nlohmann::json js;

        TO_JSON(response, js, error);
        TO_JSON(response, js, message);
        return js.dump();
    }
    catch (...)
    {
        return "";
    }
    return "";
}
