/* Copyright (c) 2018-2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of the circuit explorer for Brayns
 * <https://github.com/favreau/Brayns-UC-CircuitExplorer>
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

#include "CircuitExplorerParams.h"
#include <plugin/json.hpp>

#define FROM_JSON(PARAM, JSON, NAME) \
    PARAM.NAME = JSON[#NAME].get<decltype(PARAM.NAME)>()
#define TO_JSON(PARAM, JSON, NAME) JSON[#NAME] = PARAM.NAME;

bool from_json(Result& param, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);

        FROM_JSON(param, js, success);
        FROM_JSON(param, js, error);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

std::string to_json(const Result& param)
{
    try
    {
        nlohmann::json js;

        TO_JSON(param, js, success);
        TO_JSON(param, js, error);
        return js.dump();
    }
    catch (...)
    {
        return "";
    }
    return "";
}

bool from_json(SaveModelToCache& param, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(param, js, modelId);
        FROM_JSON(param, js, path);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool from_json(MaterialDescriptor& param, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(param, js, modelId);
        FROM_JSON(param, js, materialId);
        FROM_JSON(param, js, diffuseColor);
        FROM_JSON(param, js, specularColor);
        FROM_JSON(param, js, specularExponent);
        FROM_JSON(param, js, reflectionIndex);
        FROM_JSON(param, js, opacity);
        FROM_JSON(param, js, refractionIndex);
        FROM_JSON(param, js, emission);
        FROM_JSON(param, js, glossiness);
        FROM_JSON(param, js, simulationDataCast);
        FROM_JSON(param, js, shadingMode);
        FROM_JSON(param, js, clippingMode);
        FROM_JSON(param, js, userParameter);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool from_json(MaterialsDescriptor& param, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(param, js, modelIds);
        FROM_JSON(param, js, materialIds);
        FROM_JSON(param, js, diffuseColors);
        FROM_JSON(param, js, specularColors);
        FROM_JSON(param, js, specularExponents);
        FROM_JSON(param, js, reflectionIndices);
        FROM_JSON(param, js, opacities);
        FROM_JSON(param, js, refractionIndices);
        FROM_JSON(param, js, emissions);
        FROM_JSON(param, js, glossinesses);
        FROM_JSON(param, js, simulationDataCasts);
        FROM_JSON(param, js, shadingModes);
        FROM_JSON(param, js, clippingModes);
        FROM_JSON(param, js, userParameters);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool from_json(ModelId& param, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(param, js, modelId);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

std::string to_json(const MaterialIds& param)
{
    try
    {
        nlohmann::json js;
        TO_JSON(param, js, ids);
        return js.dump();
    }
    catch (...)
    {
        return "";
    }
    return "";
}

bool from_json(SynapseAttributes& param, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(param, js, circuitConfiguration);
        FROM_JSON(param, js, gid);
        FROM_JSON(param, js, htmlColors);
        FROM_JSON(param, js, lightEmission);
        FROM_JSON(param, js, radius);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool from_json(CircuitBoundingBox& param, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(param, js, aabb);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool from_json(ConnectionsPerValue& param, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(param, js, modelId);
        FROM_JSON(param, js, frame);
        FROM_JSON(param, js, value);
        FROM_JSON(param, js, epsilon);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool from_json(MetaballsFromSimulationValue& param, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(param, js, modelId);
        FROM_JSON(param, js, frame);
        FROM_JSON(param, js, value);
        FROM_JSON(param, js, epsilon);
        FROM_JSON(param, js, gridSize);
        FROM_JSON(param, js, threshold);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool from_json(MaterialExtraAttributes& param, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(param, js, modelId);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool from_json(LoadCellsAsInstances& param, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(param, js, connectionString);
        FROM_JSON(param, js, sqlStatement);
        FROM_JSON(param, js, name);
        FROM_JSON(param, js, description);
        FROM_JSON(param, js, morphologyFolder);
        FROM_JSON(param, js, morphologyExtension);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool from_json(ImportMorphology& param, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(param, js, connectionString);
        FROM_JSON(param, js, guid);
        FROM_JSON(param, js, filename);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool from_json(LoadCells& param, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(param, js, connectionString);
        FROM_JSON(param, js, name);
        FROM_JSON(param, js, sqlCell);
        FROM_JSON(param, js, sqlMorphology);
        FROM_JSON(param, js, sdf);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool from_json(LoadSomas& param, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(param, js, connectionString);
        FROM_JSON(param, js, sqlStatement);
        FROM_JSON(param, js, name);
        FROM_JSON(param, js, radius);
        FROM_JSON(param, js, showOrientations);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool from_json(LoadSegments& param, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(param, js, connectionString);
        FROM_JSON(param, js, sqlStatement);
        FROM_JSON(param, js, name);
        FROM_JSON(param, js, radius);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool from_json(LoadMeshes& param, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(param, js, connectionString);
        FROM_JSON(param, js, sqlStatement);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool from_json(ImportVolume& param, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(param, js, connectionString);
        FROM_JSON(param, js, guid);
        FROM_JSON(param, js, dimensions);
        FROM_JSON(param, js, spacing);
        FROM_JSON(param, js, rawFilename);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool from_json(ImportCompartmentSimulation& param, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(param, js, connectionString);
        FROM_JSON(param, js, blueConfig);
        FROM_JSON(param, js, reportName);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool from_json(CameraDefinition& param, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(param, js, origin);
        FROM_JSON(param, js, direction);
        FROM_JSON(param, js, up);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

std::string to_json(const CameraDefinition& param)
{
    try
    {
        nlohmann::json js;

        TO_JSON(param, js, origin);
        TO_JSON(param, js, direction);
        TO_JSON(param, js, up);
        TO_JSON(param, js, apertureRadius);
        TO_JSON(param, js, focusDistance);
        return js.dump();
    }
    catch (...)
    {
        return "";
    }
    return "";
}

bool from_json(AttachCellGrowthHandler& param, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(param, js, modelId);
        FROM_JSON(param, js, nbFrames);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool from_json(AttachCircuitSimulationHandler& param,
               const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(param, js, modelId);
        FROM_JSON(param, js, circuitConfiguration);
        FROM_JSON(param, js, reportName);
        FROM_JSON(param, js, synchronousMode);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool from_json(ExportFramesToDisk& param, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(param, js, path);
        FROM_JSON(param, js, format);
        FROM_JSON(param, js, quality);
        FROM_JSON(param, js, spp);
        FROM_JSON(param, js, startFrame);
        FROM_JSON(param, js, animationInformation);
        FROM_JSON(param, js, cameraInformation);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

std::string to_json(const FrameExportProgress& exportProgress)
{
    try
    {
        nlohmann::json json;
        TO_JSON(exportProgress, json, progress);
        return json.dump();
    }
    catch (...)
    {
        return "";
    }
    return "";
}

bool from_json(MakeMovieParameters& movieParams, const std::string& payload)
{
    try
    {
        auto json = nlohmann::json::parse(payload);
        FROM_JSON(movieParams, json, dimensions);
        FROM_JSON(movieParams, json, framesFolderPath);
        FROM_JSON(movieParams, json, framesFileExtension);
        FROM_JSON(movieParams, json, fpsRate);
        FROM_JSON(movieParams, json, outputMoviePath);
        FROM_JSON(movieParams, json, eraseFrames);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool from_json(AddGrid& param, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(param, js, minValue);
        FROM_JSON(param, js, maxValue);
        FROM_JSON(param, js, steps);
        FROM_JSON(param, js, radius);
        FROM_JSON(param, js, planeOpacity);
        FROM_JSON(param, js, showAxis);
        FROM_JSON(param, js, useColors);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool from_json(AddColumn& param, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(param, js, radius);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool from_json(AddSphere& param, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(param, js, name);
        FROM_JSON(param, js, center);
        FROM_JSON(param, js, radius);
        FROM_JSON(param, js, color);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool from_json(AddPill& param, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(param, js, name);
        FROM_JSON(param, js, type);
        FROM_JSON(param, js, p1);
        FROM_JSON(param, js, p2);
        FROM_JSON(param, js, radius1);
        FROM_JSON(param, js, radius2);
        FROM_JSON(param, js, color);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool from_json(AddCylinder& param, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(param, js, name);
        FROM_JSON(param, js, center);
        FROM_JSON(param, js, up);
        FROM_JSON(param, js, radius);
        FROM_JSON(param, js, color);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool from_json(AddBox& param, const std::string& payload)
{
    try
    {
        auto js = nlohmann::json::parse(payload);
        FROM_JSON(param, js, name);
        FROM_JSON(param, js, minCorner);
        FROM_JSON(param, js, maxCorner);
        FROM_JSON(param, js, color);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

std::string to_json(const AddShapeResult& addResult)
{
    try
    {
        nlohmann::json json;
        TO_JSON(addResult, json, id);
        TO_JSON(addResult, json, error);
        TO_JSON(addResult, json, message);
        return json.dump();
    }
    catch (...)
    {
        return "";
    }
    return "";
}
