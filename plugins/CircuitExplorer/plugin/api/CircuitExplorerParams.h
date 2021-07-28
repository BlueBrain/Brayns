/* Copyright (c) 2018-2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Authors: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                      Nadir Román Guerrero <nadir.romanguerrero@epfl.ch>
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
#include <brayns/network/json/ActionMessage.h>

struct SaveModelToCache : public brayns::Message
{
    MESSAGE_BEGIN(SaveModelToCache)
    MESSAGE_ENTRY(uint64_t, modelId, "The model to save to a cache file")
    MESSAGE_ENTRY(std::string, path, "The path to save the cache file")
    MESSAGE_ENTRY(bool, parsed, "A flag indicating wether the parsing was successful")
    MESSAGE_ENTRY(std::string, parseError, "A descriptive string in case the parse failed")
};

struct MaterialDescriptor : public brayns::Message
{
    MESSAGE_BEGIN(MaterialDescriptor)
    MESSAGE_ENTRY(uint64_t, modelId, "The model to which this material belongs to")
    MESSAGE_ENTRY(uint64_t, materialId, "The ID that identifies this material")
    MESSAGE_ENTRY(std::vector<float>, diffuseColor,
                  "A 3 component normalized color (RGB) describing the diffuse reflection")
    MESSAGE_ENTRY(std::vector<float>, specularColor,
                  "A 3 component normalized color (RGB) describing the specular reflection")
    MESSAGE_ENTRY(double, specularExponent,
                  "The specular exponent to sharpen the specular reflection")
    MESSAGE_ENTRY(double, reflectionIndex, "The index of reflection of the material surface")
    MESSAGE_ENTRY(double, opacity, "The transparency of the material (0 to 1)")
    MESSAGE_ENTRY(double, refractionIndex, "The index of refraction of a transparent material")
    MESSAGE_ENTRY(double, emission, "The emissive property of a material")
    MESSAGE_ENTRY(double, glossiness, "The glossy component of a material")
    MESSAGE_ENTRY(bool, simulationDataCast, "Wether to cast the user parameter for simulation")
    MESSAGE_ENTRY(int32_t, shadingMode,
                  "The choosen shading mode (0 = none, 1 = diffuse, 2 = electron, 3 = cartoon, "
                  "4 = electron transparency, 5 = perlin, 6 = diffuse transparency 7 = checker)")
    MESSAGE_ENTRY(int32_t, clippingMode,
                  "The choosen material clipping mode (0 = no clipping, 1 = clip by plane, "
                  "2 = clip by sphere)")
    MESSAGE_ENTRY(double, userParameter, "A custom parameter passed to the simulation")
};

struct MaterialsDescriptor : public brayns::Message
{
    MESSAGE_BEGIN(MaterialsDescriptor)
    MESSAGE_ENTRY(std::vector<uint64_t>, modelIds,
                  "The list of models to which the list of materials belongs to")
    MESSAGE_ENTRY(std::vector<uint64_t>, materialIds,
                  "The IDs that identifies these materials (1 per model id)")
    MESSAGE_ENTRY(std::vector<float>, diffuseColors,
                  "A 3 component normalized color (RGB) describing the diffuse reflection"
                  " (1 per material)")
    MESSAGE_ENTRY(std::vector<float>, specularColors,
                  "A 3 component normalized color (RGB) describing the specular reflection"
                  " (1 per material)")
    MESSAGE_ENTRY(std::vector<float>, specularExponents,
                  "The specular exponent to sharpen the specular reflection (1 per material)")
    MESSAGE_ENTRY(std::vector<float>, reflectionIndices,
                  "The index of reflection of the material surface (1 per material)")
    MESSAGE_ENTRY(std::vector<float>, opacities,
                  "The transparency of the material (0 to 1) (1 per material)")
    MESSAGE_ENTRY(std::vector<float>, refractionIndices,
                  "The index of refraction of a transparent material (1 per material)")
    MESSAGE_ENTRY(std::vector<float>, emissions,
                  "The emissive property of a material (1 per material)")
    MESSAGE_ENTRY(std::vector<float>, glossinesses,
                  "The glossy component of a material (1 per material)")
    MESSAGE_ENTRY(std::vector<bool>, simulationDataCasts,
                  "Wether to cast the user parameter for simulation (1 per material)")
    MESSAGE_ENTRY(std::vector<int32_t>, shadingModes,
                  "The choosen shading mode (0 = none, 1 = diffuse, 2 = electron, 3 = cartoon, "
                  "4 = electron transparency, 5 = perlin, 6 = diffuse transparency 7 = checker) "
                  "(1 per material)")
    MESSAGE_ENTRY(std::vector<int32_t>, clippingModes,
                  "The choosen material clipping mode (0 = no clipping, 1 = clip by plane, "
                  "2 = clip by sphere) (1 per material)")
    MESSAGE_ENTRY(std::vector<float>, userParameters,
                  "A custom parameter passed to the simulation (1 per material)")
};

struct MaterialRangeDescriptor : public brayns::Message
{
    MESSAGE_BEGIN(MaterialRangeDescriptor)
    MESSAGE_ENTRY(uint64_t, modelId, "The model to which these materials belongs to")
    MESSAGE_ENTRY(std::vector<uint64_t>, materialIds,
                  "The IDs that identifies the materials to modify of the given"
                  " model (an empty list will modify all materials)")
    MESSAGE_ENTRY(std::vector<float>, diffuseColor,
                  "A 3 component normalized color (RGB) describing the diffuse "
                  "reflection (minimum 1)")
    MESSAGE_ENTRY(std::vector<float>, specularColor,
                  "A 3 component normalized color (RGB) describing the specular "
                  "reflection (minimum 1)")
    MESSAGE_ENTRY(double, specularExponent,
                  "The specular exponent to sharpen the specular reflection")
    MESSAGE_ENTRY(double, reflectionIndex, "The index of reflection of the material surface")
    MESSAGE_ENTRY(double, opacity, "The transparency of the material (0 to 1)")
    MESSAGE_ENTRY(double, refractionIndex, "The index of refraction of a transparent material")
    MESSAGE_ENTRY(double, emission, "The emissive property of a material")
    MESSAGE_ENTRY(double, glossiness, "The glossy component of a material")
    MESSAGE_ENTRY(bool, simulationDataCast, "Wether to cast the user parameter for simulation")
    MESSAGE_ENTRY(int32_t, shadingMode,
                  "The choosen shading mode (0 = none, 1 = diffuse, 2 = electron, 3 = cartoon, "
                  "4 = electron transparency, 5 = perlin, 6 = diffuse transparency 7 = checker)")
    MESSAGE_ENTRY(int32_t, clippingMode,
                  "The choosen material clipping mode (0 = no clipping, 1 = clip by plane, "
                  "2 = clip by sphere)")
    MESSAGE_ENTRY(double, userParameter, "A custom parameter passed to the simulation")
};

struct MaterialProperties : public brayns::Message
{
    MESSAGE_BEGIN(MaterialProperties)
    MESSAGE_ENTRY(std::vector<std::string>, properties, "List of available material property names")
    MESSAGE_ENTRY(std::vector<std::string>, propertyTypes, "Data type of the available properties")
};

struct UpdateMaterialProperties : public brayns::Message
{
    MESSAGE_BEGIN(UpdateMaterialProperties)
    MESSAGE_ENTRY(uint64_t, modelId, "Id of the model to update")
    MESSAGE_ENTRY(std::vector<uint64_t>, materialIds, "Id of the materials to update. If empty, "
                                                      "all model materials will be updated.")
    MESSAGE_ENTRY(std::vector<std::string>, propertyNames, "List of property names to update "
                                                           "in the material")
    MESSAGE_ENTRY(std::vector<std::string>, propertyValues, "Lost of all property values to "
                                                            "update in the material. Must be in the"
                                                            "same order as the property name list")

};

struct ModelId : brayns::Message
{
    MESSAGE_BEGIN(ModelId)
    MESSAGE_ENTRY(uint64_t, modelId, "The id of the model")
};

struct ModelMaterialId : public brayns::Message
{
    MESSAGE_BEGIN(ModelMaterialId)
    MESSAGE_ENTRY(uint64_t, modelId, "The id of the model")
    MESSAGE_ENTRY(uint64_t, materialId, "The id of the material")
};

struct MaterialIds : public brayns::Message
{
    MESSAGE_BEGIN(MaterialIds)
    MESSAGE_ENTRY(std::vector<uint64_t>, ids, "The list of material ids")
};

struct SynapseAttributes : public brayns::Message
{
    MESSAGE_BEGIN(SynapseAttributes)
    MESSAGE_ENTRY(std::string, circuitConfiguration, "Path to the circuit configuration file")
    MESSAGE_ENTRY(int32_t, gid, "Target cell GID")
    MESSAGE_ENTRY(std::vector<std::string>, htmlColors, "List of rgb colors in hexadecimal")
    MESSAGE_ENTRY(double, lightEmission, "Emission parameter for the synapse material")
    MESSAGE_ENTRY(double, radius, "Synapse geometry radius")
};

struct CircuitBoundingBox : public brayns::Message
{
    MESSAGE_BEGIN(CircuitBoundingBox)
    MESSAGE_ENTRY(std::vector<double>, aabb, "The bounding box definition")
};

struct ConnectionsPerValue : public brayns::Message
{
    MESSAGE_BEGIN(ConnectionsPerValue)
    MESSAGE_ENTRY(uint64_t, modelId, "The model to which apply the connections per value setting")
    MESSAGE_ENTRY(uint32_t, frame, "The frame of the simulation in which to apply")
    MESSAGE_ENTRY(double, value, "The value")
    MESSAGE_ENTRY(double, epsilon, "The value epsilon")
};

struct MetaballsFromSimulationValue : public brayns::Message
{
    MESSAGE_BEGIN(MetaballsFromSimulationValue)
    MESSAGE_ENTRY(uint64_t, modelId, "The model to which apply the metaballs setting")
    MESSAGE_ENTRY(uint32_t, frame, "The frame in which the setting will be applied")
    MESSAGE_ENTRY(double, value, "The value for the metaballs generation")
    MESSAGE_ENTRY(double, epsilon, "The value epsilon")
    MESSAGE_ENTRY(uint32_t, gridSize, "The size of a regular grid")
    MESSAGE_ENTRY(double, threshold, "The threshold")
};

struct MaterialExtraAttributes : public brayns::Message
{
    MESSAGE_BEGIN(MaterialExtraAttributes)
    MESSAGE_ENTRY(uint32_t, modelId,
                  "The model from which the materials will have the extra attributes setted")
};

struct LoadCellsAsInstances : public brayns::Message
{
    MESSAGE_BEGIN(LoadCellsAsInstances)
    MESSAGE_ENTRY(std::string, connectionString, "The sql statement to connect to the database")
    MESSAGE_ENTRY(std::string, sqlStatement, "The sql statement to query the database")
    MESSAGE_ENTRY(std::string, name, "The name")
    MESSAGE_ENTRY(std::string, description, "The description")
    MESSAGE_ENTRY(std::string, morphologyFolder, "The folder in which to find the morphologies")
    MESSAGE_ENTRY(std::string, morphologyExtension, "The morphology file format extension")
};

struct ImportMorphology : public brayns::Message
{
    MESSAGE_BEGIN(ImportMorphology)
    MESSAGE_ENTRY(std::string, connectionString, "The sql statement to connect to the database");
    MESSAGE_ENTRY(uint64_t, guid, "The global unique identifier")
    MESSAGE_ENTRY(std::string, fileName, "The path to the file of the morphology")
};

struct LoadCells : public brayns::Message
{
    MESSAGE_BEGIN(LoadCells)
    MESSAGE_ENTRY(std::string, connectionString, "The sql statement to connect to the database")
    MESSAGE_ENTRY(std::string, name, "The name")
    MESSAGE_ENTRY(std::string, sqlCell, "The sql statement to query the cells")
    MESSAGE_ENTRY(std::string, sqlMorphology, "The sql statement to query the morphologies")
    MESSAGE_ENTRY(bool, sdf, "Wether to use signed distance field functions to render the cells")
};

struct LoadSomas : public brayns::Message
{
    MESSAGE_BEGIN(LoadSomas)
    MESSAGE_ENTRY(std::string, connectionString, "The sql statement to connect to the database")
    MESSAGE_ENTRY(std::string, sqlStatement, "The sql statement to query the database")
    MESSAGE_ENTRY(std::string, name, "The name")
    MESSAGE_ENTRY(double, radius, "The radius to apply to soma rendering")
    MESSAGE_ENTRY(bool, showOrientations, "Wether to render proxies to show the orientation")
};

struct LoadSegments : public brayns::Message
{
    MESSAGE_BEGIN(LoadSegments)
    MESSAGE_ENTRY(std::string, connectionString, "The sql statement to connect to the database")
    MESSAGE_ENTRY(std::string, sqlStatement, "The sql statement to query the database")
    MESSAGE_ENTRY(std::string, name, "The name")
    MESSAGE_ENTRY(double, radius, "The radius to apply to the segments rendering")
};

struct LoadMeshes : public brayns::Message
{
    MESSAGE_BEGIN(LoadMeshes)
    MESSAGE_ENTRY(std::string, connectionString, "The sql statement to connect to the database")
    MESSAGE_ENTRY(std::string, sqlStatement, "The sql statement to query the database")
};

struct ImportVolume : public brayns::Message
{
    MESSAGE_BEGIN(ImportVolume)
    MESSAGE_ENTRY(std::string, connectionString, "The sql statement to connect to the database")
    MESSAGE_ENTRY(uint64_t, guid, "The global unique identifier")
    MESSAGE_ENTRY(std::vector<uint32_t>, dimensions, "The volume dimensions")
    MESSAGE_ENTRY(std::vector<float>, spacing, "The volume spacing")
    MESSAGE_ENTRY(std::string, rawFilename, "The path to the volume file")
};

struct ImportCompartmentSimulation : public brayns::Message
{
    MESSAGE_BEGIN(ImportCompartmentSimulation)
    MESSAGE_ENTRY(std::string, connectionString, "The sql statement to connect to the database")
    MESSAGE_ENTRY(std::string, blueConfig,
                  "The path to the Blue Brain configuration file with the report")
    MESSAGE_ENTRY(std::string, reportName, "The name of the report to load")
};

struct CameraDefinition : public brayns::Message
{
    MESSAGE_BEGIN(CameraDefinition)
    MESSAGE_ENTRY(std::vector<double>, origin, "The position of the camera")
    MESSAGE_ENTRY(std::vector<double>, direction,
                  "A normalized vector in the direction the camera is facing")
    MESSAGE_ENTRY(std::vector<double>, up,
                  "A normalized vector, perpendicular to the direction, that points to the camera"
                  " upwards")
    MESSAGE_ENTRY(double, apertureRadius, "The camera aperture")
    MESSAGE_ENTRY(double, focusDistance,
                  "The distance from the origin, in the direction, at which the camera will focus")
};

struct AttachCellGrowthHandler : public brayns::Message
{
    MESSAGE_BEGIN(AttachCellGrowthHandler)
    MESSAGE_ENTRY(uint64_t, modelId, "The model to which to attach the handler")
    MESSAGE_ENTRY(uint64_t, nbFrames, "The number of frames to perform the growth")
};

struct AttachCircuitSimulationHandler : public brayns::Message
{
    MESSAGE_BEGIN(AttachCircuitSimulationHandler)
    MESSAGE_ENTRY(uint64_t, modelId, "The model to which to attach the handler")
    MESSAGE_ENTRY(std::string, circuitConfiguration,
                  "The path to the Blue Brain config file which contains the simulation")
    MESSAGE_ENTRY(std::string, reportName, "The name of the report to attach")
    MESSAGE_ENTRY(bool, synchronousMode, "Wether to perform the load synchronously (blocking)")
};

struct ExportFramesToDisk : public brayns::Message
{
    MESSAGE_BEGIN(ExportFramesToDisk)
    MESSAGE_ENTRY(std::string, path, "Path to the directory where the frames will be saved")
    MESSAGE_ENTRY(std::string, format, "The image format (PNG or JPEG)")
    MESSAGE_ENTRY(bool, nameAfterStep, "Name the file on disk after the simulation step index")
    MESSAGE_ENTRY(uint32_t, quality, "The quality at which the images will be stored")
    MESSAGE_ENTRY(uint32_t, spp,
                  "Samples per pixels (The more, the better visual result and the slower the"
                  " rendering)")
    MESSAGE_ENTRY(uint32_t, startFrame, "The frame at which to start exporting frames")
    MESSAGE_ENTRY(std::vector<uint64_t>, animationInformation, "A list of frame numbers to render")
    MESSAGE_ENTRY(std::vector<double>, cameraInformation,
                  "A list of camera definitions. Each camera definition contains origin, "
                  "direction, up, apperture and radius. (1 entry per animation information entry)")
};

struct FrameExportProgress : public brayns::Message
{
    MESSAGE_BEGIN(FrameExportProgress)
    MESSAGE_ENTRY(double, progress, "The normalized progress (0.0 to 1.0) of the last export "
                                    "frames to disk request")
};

struct ExportLayerToDisk : public brayns::Message
{
    MESSAGE_BEGIN(ExportLayerToDisk)
    MESSAGE_ENTRY(std::string, path, "Path where to store the frames")
    MESSAGE_ENTRY(std::string, name, "Name to give to the layer frames")
    MESSAGE_ENTRY(uint32_t, startFrame, "The frame number of the first frame to store "
                                        "(For instance: name00025.png")
    MESSAGE_ENTRY(uint32_t, framesCount, "Number of frames to store, starting at startFrame")
    MESSAGE_ENTRY(std::string, data, "Base64 layer image data to store on every frame")
};

struct ExportLayerToDiskResult : public brayns::Message
{
    MESSAGE_BEGIN(ExportLayerToDiskResult)
    MESSAGE_ENTRY(std::vector<uint32_t>, frames, "List of frames that were successfully stored"
                                                 " from the last export layer to disk request")
};

struct MakeMovieParameters : public brayns::Message
{
    MESSAGE_BEGIN(MakeMovieParameters)
    MESSAGE_ENTRY(std::vector<uint32_t>, dimensions, "Video dimensions (width,height)")
    MESSAGE_ENTRY(std::string, framesFolderPath, "Path to where to fetch the frames to create the"
                                                 " video")
    MESSAGE_ENTRY(std::string, framesFileExtension, "The extension of the frame files to fetch "
                                                    "(png, jpg)")
    MESSAGE_ENTRY(uint32_t, fpsRate, "The frames per second rate at which to create the video")
    MESSAGE_ENTRY(std::string, outputMoviePath, "The path to where the movie will be created."
                                                " Must include filename and extension")
    MESSAGE_ENTRY(bool, eraseFrames, "Wether to clean up the frame image files after generating"
                                     " the video file")
    MESSAGE_ENTRY(std::vector<std::string>, layers, "List of layer names to compose in the video. Layer name \"movie\" must be always present")
};

struct AddGrid : public brayns::Message
{
    MESSAGE_BEGIN(AddGrid)
    MESSAGE_ENTRY(double, minValue, "Negative square grid length from world origin")
    MESSAGE_ENTRY(double, maxValue, "Positive square grid length from world origin")
    MESSAGE_ENTRY(double, steps, "Number of divisions")
    MESSAGE_ENTRY(double, radius, "Radius of the cylinder that will be placed at each cell")
    MESSAGE_ENTRY(double, planeOpacity, "Opacity of the grid mesh material")
    MESSAGE_ENTRY(bool, showAxis, "Wether to show a world aligned axis")
    MESSAGE_ENTRY(bool, useColors, "Use colors on the grid axes")
};

struct AddColumn : public brayns::Message
{
    MESSAGE_BEGIN(AddColumn)
    MESSAGE_ENTRY(double, radius, "Radium of the cylinder column to add")
};

struct AnterogradeTracing : public brayns::Message
{
    MESSAGE_BEGIN(AnterogradeTracing)
    MESSAGE_ENTRY(uint64_t, modelId, "Model where to perform the neuronal tracing")
    MESSAGE_ENTRY(std::vector<uint32_t>, cellGids, "List of cell GIDs to use a source of the "
                                                   "tracing")
    MESSAGE_ENTRY(std::vector<uint32_t>, targetCellGids, "List of cells GIDs which are the result"
                                                         " of the given tracing mode")
    MESSAGE_ENTRY(std::vector<double>, sourceCellColor, "A 4 component normalized color (RGBA) to "
                                                        "apply to the source cell geometry")
    MESSAGE_ENTRY(std::vector<double>, connectedCellsColor, "A 4 component normalized color (RGBA) "
                                                            "to apply to the target cells geometry")
    MESSAGE_ENTRY(std::vector<double>, nonConnectedCellsColor, "A 4 component normalized color (RGBA)"
                                                               " to apply to the rest of cells")
};

struct AddSphere : public brayns::Message
{
    MESSAGE_BEGIN(AddSphere)
    MESSAGE_ENTRY(std::string, name, "Name to give to the added model")
    MESSAGE_ENTRY(std::vector<double>, center, "The coordinates of the sphere center (X,Y,Z)")
    MESSAGE_ENTRY(double, radius, "Radius of the sphere")
    MESSAGE_ENTRY(std::vector<double>, color, "A 4 component normalized color (RGBA) to apply"
                                              " to the sphere surface")
};

struct AddPill : public brayns::Message
{
    MESSAGE_BEGIN(AddPill)
    MESSAGE_ENTRY(std::string, name, "Name to give to the added model")
    MESSAGE_ENTRY(std::string, type, "Type of pill (pill, conepill or sigmoidpill)")
    MESSAGE_ENTRY(std::vector<double>, p1, "Center of the lower pill circunference")
    MESSAGE_ENTRY(std::vector<double>, p2, "Center of the upper pill circunference")
    MESSAGE_ENTRY(double, radius1, "Radius of the lower pill circunference")
    MESSAGE_ENTRY(double, radius2, "Radius of the upper pill circunference")
    MESSAGE_ENTRY(std::vector<double>, color, "A 4 component normalized color (RGBA) to apply"
                                              " to the pill surface")
};

struct AddCylinder : public brayns::Message
{
    MESSAGE_BEGIN(AddCylinder)
    MESSAGE_ENTRY(std::string, name, "Name to give to the added model")
    MESSAGE_ENTRY(std::vector<double>, center, "Center of the lower cylinder circunference")
    MESSAGE_ENTRY(std::vector<double>, up, "Center of the upper cylinder circunference")
    MESSAGE_ENTRY(double, radius, "Radius of the cylinder")
    MESSAGE_ENTRY(std::vector<double>, color, "A 4 component normalized color (RGBA) to apply"
                                              " to the cylinder surface")
};

struct AddBox : public brayns::Message
{
    MESSAGE_BEGIN(AddBox)
    MESSAGE_ENTRY(std::string, name, "Name to give to the added model")
    MESSAGE_ENTRY(std::vector<double>, minCorner, "Axis aligned minimum bound of the box")
    MESSAGE_ENTRY(std::vector<double>, maxCorner, "Axis aligned maximum bound of the box")
    MESSAGE_ENTRY(std::vector<double>, color, "A 4 component normalized color (RGBA) to apply"
                                              " to the box surface")
};

struct AddShapeResult : public brayns::Message
{
    MESSAGE_BEGIN(AddShapeResult)
    MESSAGE_ENTRY(uint64_t, id, "The id of the added model")
};

struct RemapCircuit : public brayns::Message
{
    MESSAGE_BEGIN(RemapCircuit)
    MESSAGE_ENTRY(uint64_t, modelId, "The model to remap")
    MESSAGE_ENTRY(std::string, scheme, "Color scheme to remap a circuit to (Possible values: "
                                       "\"By id\", \"By layer\", \"By mtype\", \"By etype\", "
                                       "\"By target\")")
};

struct RemapCircuitResult: public brayns::Message
{
    MESSAGE_BEGIN(RemapCircuitResult)
    MESSAGE_ENTRY(bool, updated, "Wether the colors of the model changed because of the "
                                 "remapping.")
};

struct ColorCells : public brayns::Message
{
    MESSAGE_BEGIN(ColorCells)
    MESSAGE_ENTRY(uint64_t, modelId, "The model to which apply the coloring")
    MESSAGE_ENTRY(std::vector<std::string>, gids, "List of cell GIDs to color")
    MESSAGE_ENTRY(std::vector<double>, colors, "List of RGB colors, in the same order as "
                                               "the GIDs were specified")
};

struct MirrorModel : public brayns::Message
{
    MESSAGE_BEGIN(MirrorModel)
    MESSAGE_ENTRY(uint64_t, modelId, "The model to which to mirror")
    MESSAGE_ENTRY(std::vector<double>, mirrorAxis, "The axis to use for mirroring")
};

struct CircuitThickness : public brayns::Message
{
    MESSAGE_BEGIN(CircuitThickness)
    MESSAGE_ENTRY(uint64_t, modelId, "The model to which to apply the thickness change")
    MESSAGE_ENTRY(double, radiusMultiplier, "The facto by which to multiply the geometry radiuses")
};

#endif // CIRCUITEXLORERPARAMS_H
