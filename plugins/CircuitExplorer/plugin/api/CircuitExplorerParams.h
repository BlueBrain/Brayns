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

#ifndef CIRCUITEXPLORERPARAMS_H
#define CIRCUITEXPLORERPARAMS_H

#include "../../common/types.h"
#include <brayns/common/types.h>

struct Result
{
    bool success{false};
    std::string error;
};

std::string to_json(const Result& param);

struct MessageResult
{
    int error{0};
    std::string message{""};
};

std::string to_json(const MessageResult& result);

/** Save model to cache */
struct SaveModelToCache
{
    int32_t modelId;
    std::string path;
    bool parsed{true};
    std::string parseError{""};
};

bool from_json(SaveModelToCache& modelSave, const std::string& payload);

struct MaterialDescriptor : public MessageResult
{
    int32_t modelId;
    int32_t materialId;
    std::vector<float> diffuseColor;
    std::vector<float> specularColor;
    float specularExponent;
    float reflectionIndex;
    float opacity;
    float refractionIndex;
    float emission;
    float glossiness;
    bool simulationDataCast;
    int32_t shadingMode;
    int32_t clippingMode;
    float userParameter;
    bool parsed{true};
    std::string parseError{""};
};

bool from_json(MaterialDescriptor& materialDescriptor,
               const std::string& payload);
std::string to_json(const MaterialDescriptor& payload);

struct MaterialsDescriptor
{
    std::vector<int32_t> modelIds;
    std::vector<int32_t> materialIds;
    std::vector<float> diffuseColors;
    std::vector<float> specularColors;
    std::vector<float> specularExponents;
    std::vector<float> reflectionIndices;
    std::vector<float> opacities;
    std::vector<float> refractionIndices;
    std::vector<float> emissions;
    std::vector<float> glossinesses;
    std::vector<bool> simulationDataCasts;
    std::vector<int32_t> shadingModes;
    std::vector<int32_t> clippingModes;
    std::vector<float> userParameters;
    bool parsed{true};
    std::string parseError{""};
};

bool from_json(MaterialsDescriptor& materialsDescriptor,
               const std::string& payload);

struct MaterialRangeDescriptor
{
    int32_t modelId;
    std::vector<int32_t> materialIds;
    std::vector<float> diffuseColor;
    std::vector<float> specularColor;
    float specularExponent;
    float reflectionIndex;
    float opacity;
    float refractionIndex;
    float emission;
    float glossiness;
    bool simulationDataCast;
    int32_t shadingMode;
    int32_t clippingMode;
    float userParameter;
    bool parsed{true};
    std::string parseError{""};
};

bool from_json(MaterialRangeDescriptor& materialRangeDescriptor,
               const std::string& payload);

// Material IDs for a given model
struct ModelId
{
    size_t modelId;
    bool parsed{true};
    std::string parseError{""};
};
bool from_json(ModelId& modelId, const std::string& payload);

struct ModelMaterialId
{
    size_t modelId;
    size_t materialId;
    bool parsed{true};
    std::string parseError{""};
};
bool from_json(ModelMaterialId& mmId, const std::string& payload);

struct MaterialIds : public MessageResult
{
    std::vector<size_t> ids;
};

std::string to_json(const MaterialIds& param);

// Synapse attributes
struct SynapseAttributes
{
    std::string circuitConfiguration;
    int32_t gid;
    std::vector<std::string> htmlColors;
    float lightEmission;
    float radius;
    bool parsed{true};
    std::string parseError{""};
};

bool from_json(SynapseAttributes& synapseAttributes,
               const std::string& payload);

/** Circuit bounding box */
struct CircuitBoundingBox
{
    std::vector<double> aabb{0, 0, 0, 0, 0, 0};
    bool parsed{true};
    std::string parseError{""};
};

bool from_json(CircuitBoundingBox& circuitBoundingBox,
               const std::string& payload);

/** Connections per value */
struct ConnectionsPerValue
{
    int32_t modelId;
    int32_t frame;
    double value;
    double epsilon;
    bool parsed{true};
    std::string parseError{""};
};

bool from_json(ConnectionsPerValue& connectionsPerValue,
               const std::string& payload);

/** Metaballs per simulation value */
struct MetaballsFromSimulationValue
{
    int32_t modelId;
    int32_t frame;
    double value;
    double epsilon;
    int32_t gridSize;
    double threshold;
    bool parsed{true};
    std::string parseError{""};
};

bool from_json(MetaballsFromSimulationValue& param, const std::string& payload);

/** Set extra attributes to materials */
struct MaterialExtraAttributes
{
    int32_t modelId;
    bool parsed{true};
    std::string parseError{""};
};

bool from_json(MaterialExtraAttributes& param, const std::string& payload);

// DB API
struct LoadCellsAsInstances
{
    std::string connectionString;
    std::string sqlStatement;
    std::string name;
    std::string description;
    std::string morphologyFolder;
    std::string morphologyExtension;
    bool parsed{true};
    std::string parseError{""};
};
bool from_json(LoadCellsAsInstances& param, const std::string& payload);

struct ImportMorphology
{
    std::string connectionString;
    uint64_t guid;
    std::string filename;
    bool parsed{true};
    std::string parseError{""};
};
bool from_json(ImportMorphology& param, const std::string& payload);

struct LoadCells
{
    std::string connectionString;
    std::string name;
    std::string sqlCell;
    std::string sqlMorphology;
    bool sdf;
    bool parsed{true};
    std::string parseError{""};
};
bool from_json(LoadCells& param, const std::string& payload);

struct LoadSomas
{
    std::string connectionString;
    std::string sqlStatement;
    std::string name;
    float radius{1.f};
    bool showOrientations;
    bool parsed{true};
    std::string parseError{""};
};
bool from_json(LoadSomas& param, const std::string& payload);

struct LoadSegments
{
    std::string connectionString;
    std::string sqlStatement;
    std::string name;
    float radius{1.f};
    bool parsed{true};
    std::string parseError{""};
};
bool from_json(LoadSegments& param, const std::string& payload);

struct LoadMeshes
{
    std::string connectionString;
    std::string sqlStatement;
    bool parsed{true};
    std::string parseError{""};
};
bool from_json(LoadMeshes& param, const std::string& payload);

struct ImportVolume
{
    std::string connectionString;
    uint64_t guid;
    std::vector<uint16_t> dimensions;
    std::vector<float> spacing;
    std::string rawFilename;
    bool parsed{true};
    std::string parseError{""};
};
bool from_json(ImportVolume& param, const std::string& payload);

struct ImportCompartmentSimulation
{
    std::string connectionString;
    std::string blueConfig;
    std::string reportName;
    bool parsed{true};
    std::string parseError{""};
};
bool from_json(ImportCompartmentSimulation& param, const std::string& payload);

struct CameraDefinition : public MessageResult
{
    std::vector<double> origin;
    std::vector<double> direction;
    std::vector<double> up;
    double apertureRadius;
    double focusDistance;
    bool parsed{true};
    std::string parseError{""};
};
bool from_json(CameraDefinition& param, const std::string& payload);
std::string to_json(const CameraDefinition& param);

struct AttachCellGrowthHandler
{
    uint64_t modelId;
    uint64_t nbFrames;
    bool parsed{true};
    std::string parseError{""};
};
bool from_json(AttachCellGrowthHandler& param, const std::string& payload);

struct AttachCircuitSimulationHandler
{
    uint64_t modelId;
    std::string circuitConfiguration;
    std::string reportName;
    bool synchronousMode;
    bool parsed{true};
    std::string parseError{""};
};
bool from_json(AttachCircuitSimulationHandler& param,
               const std::string& payload);

struct ExportFramesToDisk
{
    std::string path;
    std::string format;
    uint16_t quality{100};
    uint16_t spp{0};
    uint16_t startFrame{0};
    std::vector<uint64_t> animationInformation;
    std::vector<double> cameraInformation;
    bool parsed{true};
    std::string parseError{""};
};
bool from_json(ExportFramesToDisk& param, const std::string& payload);

struct FrameExportProgress : public MessageResult
{
    float progress;
};
std::string to_json(const FrameExportProgress& exportProgress);

struct ExportLayerToDisk
{
    std::string path;
    std::string name;
    uint32_t startFrame;
    uint32_t framesCount;
    std::string data;
    bool parsed{true};
    std::string parseError{""};
};
bool from_json(ExportLayerToDisk& param, const std::string& payload);

struct ExportLayerToDiskResult : public MessageResult
{
    std::vector<uint32_t> frames;
};
std::string to_json(const ExportLayerToDiskResult& result);

struct MakeMovieParameters
{
    std::vector<uint16_t> dimensions;
    std::string framesFolderPath;
    std::string framesFileExtension;
    uint32_t fpsRate;
    std::string outputMoviePath;
    bool eraseFrames;
    strings layers;
    bool parsed{true};
    std::string parseError{""};
};
bool from_json(MakeMovieParameters& movieParams, const std::string& payload);

struct AddGrid
{
    float minValue;
    float maxValue;
    float steps;
    float radius;
    float planeOpacity;
    bool showAxis;
    bool useColors;
    bool parsed{true};
    std::string parseError{""};
};
bool from_json(AddGrid& param, const std::string& payload);

struct AddColumn
{
    float radius;
    bool parsed{true};
    std::string parseError{""};
};
bool from_json(AddColumn& param, const std::string& payload);

struct AnterogradeTracing
{
    int32_t modelId;
    std::vector<uint32_t> cellGIDs;
    std::vector<uint32_t> targetCellGIDs;
    std::vector<double> sourceCellColor;
    std::vector<double> connectedCellsColor;
    std::vector<double> nonConnectedCellsColor;
    bool parsed{true};
    std::string parseError{""};
};
bool from_json(AnterogradeTracing& param, const std::string& payload);

struct AddSphere
{
    std::string name;
    std::vector<float> center;
    float radius;
    std::vector<double> color;
    bool parsed{true};
    std::string parseError{""};
};
bool from_json(AddSphere& param, const std::string& payload);

struct AddPill
{
    std::string name;
    std::string type;
    std::vector<float> p1;
    std::vector<float> p2;
    float radius1;
    float radius2;
    std::vector<double> color;
    bool parsed{true};
    std::string parseError{""};
};
bool from_json(AddPill& param, const std::string& payload);

struct AddCylinder
{
    std::string name;
    std::vector<float> center;
    std::vector<float> up;
    float radius;
    std::vector<double> color;
    bool parsed{true};
    std::string parseError{""};
};
bool from_json(AddCylinder& param, const std::string& payload);

struct AddBox
{
    std::string name;
    std::vector<float> minCorner;
    std::vector<float> maxCorner;
    std::vector<double> color;
    bool parsed{true};
    std::string parseError{""};
};
bool from_json(AddBox& param, const std::string& payload);

struct AddShapeResult : public MessageResult
{
    size_t id;
};
std::string to_json(const AddShapeResult& addResult);

struct RemapCircuit
{
    uint32_t modelId;
    std::string scheme;
    bool parsed{true};
    std::string parseError{""};
};
bool from_json(RemapCircuit& param, const std::string& payload);

#endif // CIRCUITVIEWERPARAMS_H
