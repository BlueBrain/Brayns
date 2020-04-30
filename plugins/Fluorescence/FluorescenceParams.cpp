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
#include "FluorescenceParams.h"
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

bool from_json(AddDiskSensorRequest& request, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(request, js, rotation);
        FROM_JSON(request, js, translation);
        FROM_JSON(request, js, scale);
        FROM_JSON(request, js, reference);
        FROM_JSON(request, js, xpixels);
        FROM_JSON(request, js, ypixels);
        FROM_JSON(request, js, fov);
        FROM_JSON(request, js, height);
        FROM_JSON(request, js, radius);
        FROM_JSON(request, js, innerRadius);
        FROM_JSON(request, js, phi);
    }
    catch (const std::exception& except)
    {
        request.parsed = false;
        request.parseMessage = std::string(except.what());
    }

    return true;
}

bool from_json(AddRectangleSensorRequest& request, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(request, js, rotation);
        FROM_JSON(request, js, translation);
        FROM_JSON(request, js, scale);
        FROM_JSON(request, js, reference);
        FROM_JSON(request, js, xpixels);
        FROM_JSON(request, js, ypixels);
        FROM_JSON(request, js, fov);
        FROM_JSON(request, js, height);
        FROM_JSON(request, js, x);
        FROM_JSON(request, js, y);
    }
    catch (const std::exception& except)
    {
        request.parsed = false;
        request.parseMessage = std::string(except.what());
    }

    return true;
}

bool from_json(AddFluorescentVolume& request, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(request, js, p0);
        FROM_JSON(request, js, p1);
        FROM_JSON(request, js, name);
        FROM_JSON(request, js, fex);
        FROM_JSON(request, js, fem);
        FROM_JSON(request, js, epsilon);
        FROM_JSON(request, js, c);
        FROM_JSON(request, js, yield);
        FROM_JSON(request, js, gf);
    }
    catch (const std::exception& except)
    {
        request.parsed = false;
        request.parseMessage = std::string(except.what());
    }

    return true;
}

bool from_json(AddFluorescentAnnotatedVolume& request, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(request, js, p0);
        FROM_JSON(request, js, p1);
        FROM_JSON(request, js, name);
        FROM_JSON(request, js, g);
        FROM_JSON(request, js, ntags);
        FROM_JSON(request, js, fexs);
        FROM_JSON(request, js, fems);
        FROM_JSON(request, js, epsilons);
        FROM_JSON(request, js, cs);
        FROM_JSON(request, js, yields);
        FROM_JSON(request, js, gfs);
    }
    catch (const std::exception& except)
    {
        request.parsed = false;
        request.parseMessage = std::string(except.what());
    }

    return true;
}

bool from_json(AddFluorescentBinaryVolume& request, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(request, js, p0);
        FROM_JSON(request, js, p1);
        FROM_JSON(request, js, name);
        FROM_JSON(request, js, g);
        FROM_JSON(request, js, fex);
        FROM_JSON(request, js, fem);
        FROM_JSON(request, js, epsilon);
        FROM_JSON(request, js, c);
        FROM_JSON(request, js, yield);
        FROM_JSON(request, js, gf);
        FROM_JSON(request, js, prefix);
    }
    catch (const std::exception& except)
    {
        request.parsed = false;
        request.parseMessage = std::string(except.what());
    }

    return true;
}

bool from_json(AddFluorescentGridFromFileVolume& request, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(request, js, p0);
        FROM_JSON(request, js, p1);
        FROM_JSON(request, js, name);
        FROM_JSON(request, js, g);
        FROM_JSON(request, js, fex);
        FROM_JSON(request, js, fem);
        FROM_JSON(request, js, epsilon);
        FROM_JSON(request, js, c);
        FROM_JSON(request, js, yield);
        FROM_JSON(request, js, gf);
        FROM_JSON(request, js, prefix);
    }
    catch (const std::exception& except)
    {
        request.parsed = false;
        request.parseMessage = std::string(except.what());
    }

    return true;
}

bool from_json(AddFluorescentGridVolume& request, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(request, js, p0);
        FROM_JSON(request, js, p1);
        FROM_JSON(request, js, name);
        FROM_JSON(request, js, g);
        FROM_JSON(request, js, fex);
        FROM_JSON(request, js, fem);
        FROM_JSON(request, js, epsilon);
        FROM_JSON(request, js, c);
        FROM_JSON(request, js, yield);
        FROM_JSON(request, js, gf);
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

bool from_json(AddFluorescentScatteringVolume& request, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(request, js, p0);
        FROM_JSON(request, js, p1);
        FROM_JSON(request, js, name);
        FROM_JSON(request, js, g);
        FROM_JSON(request, js, absorption);
        FROM_JSON(request, js, scattering);
        FROM_JSON(request, js, fex);
        FROM_JSON(request, js, fem);
        FROM_JSON(request, js, mweight);
        FROM_JSON(request, js, epsilon);
        FROM_JSON(request, js, c);
        FROM_JSON(request, js, yield);
        FROM_JSON(request, js, sscale);
        FROM_JSON(request, js, fscale);
        FROM_JSON(request, js, gf);
        FROM_JSON(request, js, density);
        FROM_JSON(request, js, Le);
    }
    catch (const std::exception& except)
    {
        request.parsed = false;
        request.parseMessage = std::string(except.what());
    }

    return true;
}

bool from_json(AddFluorescentScatteringGridFromFileVolume& request, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(request, js, p0);
        FROM_JSON(request, js, p1);
        FROM_JSON(request, js, name);
        FROM_JSON(request, js, g);
        FROM_JSON(request, js, absorption);
        FROM_JSON(request, js, scattering);
        FROM_JSON(request, js, fex);
        FROM_JSON(request, js, fem);
        FROM_JSON(request, js, mweight);
        FROM_JSON(request, js, epsilon);
        FROM_JSON(request, js, c);
        FROM_JSON(request, js, yield);
        FROM_JSON(request, js, sscale);
        FROM_JSON(request, js, fscale);
        FROM_JSON(request, js, gf);
        FROM_JSON(request, js, prefix);
        FROM_JSON(request, js, Le);
    }
    catch (const std::exception& except)
    {
        request.parsed = false;
        request.parseMessage = std::string(except.what());
    }

    return true;
}

bool from_json(AddFluorescentScatteringGridVolume& request, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(request, js, p0);
        FROM_JSON(request, js, p1);
        FROM_JSON(request, js, name);
        FROM_JSON(request, js, g);
        FROM_JSON(request, js, absorption);
        FROM_JSON(request, js, scattering);
        FROM_JSON(request, js, fex);
        FROM_JSON(request, js, fem);
        FROM_JSON(request, js, mweight);
        FROM_JSON(request, js, epsilon);
        FROM_JSON(request, js, c);
        FROM_JSON(request, js, yield);
        FROM_JSON(request, js, sscale);
        FROM_JSON(request, js, fscale);
        FROM_JSON(request, js, gf);
        FROM_JSON(request, js, Le);
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

std::string to_json(const RequestResponse& response)
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
