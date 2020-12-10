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

#ifndef CIRCUIT_EXPLORER_TYPES_H
#define CIRCUIT_EXPLORER_TYPES_H

#include <brayns/common/PropertyMap.h>
#include <brayns/common/geometry/Cone.h>
#include <brayns/common/geometry/Cylinder.h>
#include <brayns/common/geometry/SDFGeometry.h>
#include <brayns/common/geometry/Sphere.h>
#include <brayns/common/mathTypes.h>
#include <brayns/common/types.h>
#include <brayns/engine/Model.h>

#include <memory>
#include <vector>

#include <glm/gtx/matrix_decompose.hpp>

/** Additional marterial attributes */
const std::string MATERIAL_PROPERTY_CAST_USER_DATA = "cast_simulation_data";
const std::string MATERIAL_PROPERTY_SHADING_MODE = "shading_mode";
const std::string MATERIAL_PROPERTY_CLIPPING_MODE = "clipping_mode";
const std::string MATERIAL_PROPERTY_USER_PARAMETER = "user_parameter";

/** Voltage Simulation handlers */
class AbstractUserDataHandler;
using AbstractUserDataHandlerPtr = std::shared_ptr<AbstractUserDataHandler>;

/** Voltage Simulation handlers */
class VoltageSimulationHandler;
using VoltageSimulationHandlerPtr = std::shared_ptr<VoltageSimulationHandler>;

/** Spike Simulation handlers */
class SpikeSimulationHandler;
using SpikeSimulationHandlerPtr = std::shared_ptr<SpikeSimulationHandler>;

/** Circuit color scheme */
enum class CircuitColorScheme
{
    none = 0,
    by_id = 1,
    by_type = 2,
    by_layer = 3,
    by_mtype = 4,
    by_etype = 5,
    by_target = 6,
    single_color = 7
};

/** Morphology color scheme */
enum class MorphologyColorScheme
{
    none = 0,
    neuron_by_segment_type = 1
};

enum class ReportType
{
    undefined = 0,
    voltages_from_file = 1,
    spikes = 2
};

/** User data type */
enum class UserDataType
{
    undefined = 0,
    simulation_offset = 1,
    distance_to_soma = 2
};

enum class MorphologyQuality
{
    low = 0,
    medium = 1,
    high = 2
};

namespace
{
/* Returns a mapping from a name to an enum type. */
template <typename EnumT>
std::vector<std::pair<std::string, EnumT>> enumerateMap();

/* Convert a string to an enum. */
template <typename EnumT>
inline EnumT stringToEnum(const std::string& v)
{
    for (const auto& p : enumerateMap<EnumT>())
        if (p.first == v)
            return p.second;

    throw std::runtime_error("Could not match enum '" + v + "'");
    return static_cast<EnumT>(0);
}

/* Convert an enum to a string. */
template <typename EnumT>
inline std::string enumToString(const EnumT v)
{
    for (const auto& p : enumerateMap<EnumT>())
        if (p.second == v)
            return p.first;

    throw std::runtime_error("Could not match enum");
    return "Invalid";
}

/* Returns all names for given enum type 'EnumT' */
template <typename EnumT>
inline std::vector<std::string> enumerateNames()
{
    std::vector<std::string> v;
    for (const auto& p : enumerateMap<EnumT>())
        v.push_back(p.first);
    return v;
}

template <>
inline std::vector<std::pair<std::string, CircuitColorScheme>> enumerateMap()
{
    return {{"None", CircuitColorScheme::none},
            {"By id", CircuitColorScheme::by_id},
            {"By layer", CircuitColorScheme::by_layer},
            {"By mtype", CircuitColorScheme::by_mtype},
            {"By etype", CircuitColorScheme::by_etype},
            {"By target", CircuitColorScheme::by_target},
            {"Single color", CircuitColorScheme::single_color}};
}

template <>
inline std::vector<std::pair<std::string, ReportType>> enumerateMap()
{
    return {{"Undefined", ReportType::undefined},
            {"Voltages from file", ReportType::voltages_from_file},
            {"Spikes", ReportType::spikes}};
}

template <>
inline std::vector<std::pair<std::string, UserDataType>> enumerateMap()
{
    return {{"Undefined", UserDataType::undefined},
            {"Simulation offset", UserDataType::simulation_offset},
            {"Distance to soma", UserDataType::distance_to_soma}};
}

template <>
inline std::vector<std::pair<std::string, MorphologyColorScheme>> enumerateMap()
{
    return {{"None", MorphologyColorScheme::none},
            {"By segment type", MorphologyColorScheme::neuron_by_segment_type}};
}

template <>
inline std::vector<std::pair<std::string, MorphologyQuality>> enumerateMap()
{
    return {{"Low", MorphologyQuality::low},
            {"Medium", MorphologyQuality::medium},
            {"High", MorphologyQuality::high}};
}

template <>
inline std::vector<std::pair<std::string, bool>> enumerateMap()
{
    return {{"On", true}, {"Off", false}};
}

std::string CIRCUIT_ON_OFF[2] = {"off", "on"};
} // namespace

// clang-format off
const brayns::Property PROP_DB_CONNECTION_STRING = {
    "000DbConnectionString", std::string(""),
    {"Connection string to the database"}};
const brayns::Property PROP_DENSITY = {
    "001Density", 1.0,
    {"Density of cells in the circuit in percent",
     "Will not have effect if a list of GIDs is specified."}};
const brayns::Property PROP_RANDOM_SEED = {
    "002RandomSeed", 0.0,
    {"Random seed for target subsetting"}};
const brayns::Property PROP_TARGETS = {
    "010Targets",  std::string(""),
    {"Circuit targets [comma separated list of labels]"}};
const brayns::Property PROP_GIDS = {
    "011Gids", std::string(""),
    {"Circuit GIDs [comma separated list of GIDs]",
     "Invalidates the density parameter when used."}};
const brayns::Property PROP_PRESYNAPTIC_NEURON_GID = {
    "012PreNeuron",  std::string(""),
    {"Pre-synaptic neuron GID"}};
const brayns::Property PROP_POSTSYNAPTIC_NEURON_GID = {
    "013PostNeuron",  std::string(""),
    {"Post-synaptic neuron GID"}};
const brayns::Property PROP_REPORT{
    "020Report", std::string(),
    {"Circuit report"}};
const brayns::Property PROP_REPORT_TYPE = {
    "021ReportType", enumToString(ReportType::undefined),
    enumerateNames<ReportType>(),
    {"Type of simulation report"}};
const brayns::Property PROP_USER_DATA_TYPE = {
    "022UserDataType", enumToString(UserDataType::undefined),
    enumerateNames<UserDataType>(),
    {"Type of data attached to morphology segments"}};
const brayns::Property PROP_SYNCHRONOUS_MODE = {
    "023SynchronousMode", false, {"Synchronous mode"}};
const brayns::Property PROP_SPIKE_TRANSITION_TIME = {
    "024SpikeTransitionTime", 1.0, {"Growth and fade of spike in seconds"}};
const brayns::Property PROP_CIRCUIT_COLOR_SCHEME = {
    "030CircuitColorScheme", enumToString(CircuitColorScheme::single_color),
    enumerateNames<CircuitColorScheme>(),
    {"Color scheme to be applied to the circuit"}};
const brayns::Property PROP_MESH_FOLDER = {
    "040MeshFolder", std::string(), {"Folder constaining meshes"}};
const brayns::Property PROP_MESH_FILENAME_PATTERN = {
    "041MeshFilenamePattern", std::string("mesh_{gid}.obj"), {"File name pattern for meshes"}};
const brayns::Property PROP_MESH_TRANSFORMATION = {
    "042MeshTransformation", false, {"Apply circuit transformation to meshes"}};
const brayns::Property PROP_RADIUS_MULTIPLIER = {
    "050RadiusMultiplier", double(1.0),
    {"Multiplier applied to morphology radius"}};
const brayns::Property PROP_RADIUS_CORRECTION = {
    "051RadiusCorrection", double(0.0),
    {"Value overrideing the radius of the morphology"}};
const brayns::Property PROP_SECTION_TYPE_SOMA = {
    "052SectionTypeSoma", true,
    {"Soma"}};
const brayns::Property PROP_SECTION_TYPE_AXON = {
    "053SectionTypeAxon", true,
    {"Axon"}};
const brayns::Property PROP_SECTION_TYPE_DENDRITE = {
    "054SectionTypeDendrite", true,
    {"Dendrite"}};
const brayns::Property PROP_SECTION_TYPE_APICAL_DENDRITE = {
    "055SectionTypeApicalDendrite", true,
    {"Apical Dendrite"}};
const brayns::Property PROP_USE_SDF_GEOMETRY = {
    "060UseSdfgeometry", true,
    { "Use signed distance field geometry"}};
const brayns::Property PROP_DAMPEN_BRANCH_THICKNESS_CHANGERATE = {
    "061DampenBranchThicknessChangerate", true,
    {"Dampen branch thickness changerate"}};
const brayns::Property PROP_USE_REALISTIC_SOMA = {
    "070RealisticSoma", false,
    {"Uses metaballs to build somata geometry"}};
const brayns::Property PROP_METABALLS_SAMPLES_FROM_SOMA = {
    "071MetaballsSamplesFromSoma", 5,
    {"Number of section samples used to compute metaballs"}};
const brayns::Property PROP_METABALLS_GRID_SIZE = {
    "072MetaballsGridSize", 20,
    {"Size of the grid uarraysed to compute metaballs"}};
const brayns::Property PROP_METABALLS_THRESHOLD = {
    "073MetaballsThreshold", 1.,
    {"Threshold used to compute metaballs"}};
const brayns::Property PROP_MORPHOLOGY_COLOR_SCHEME = {
    "080MorphologyColorScheme", enumToString(MorphologyColorScheme::none),
    enumerateNames<MorphologyColorScheme>(),
    {"Color scheme to be applied to the morphology"}};
const brayns::Property PROP_MORPHOLOGY_QUALITY = {
    "090MorphologyQuality", enumToString(MorphologyQuality::high),
    enumerateNames<MorphologyQuality>(),
    {"Quality of the morphology"}};
const brayns::Property PROP_MORPHOLOGY_MAX_DISTANCE_TO_SOMA = {
    "091MaxDistanceToSoma", std::numeric_limits<double>::max(),
    {"Maximum distance to soma"}};
const brayns::Property PROP_CELL_CLIPPING = {
    "100CellClipping", false,
    {"Clip cells according to scene-defined clipping planes"}};
const brayns::Property PROP_AREAS_OF_INTEREST = {
    "101AreasOfInterest", 0,
    {"Loads only one cell per area of interest"}};
const brayns::Property PROP_SYNAPSE_RADIUS = {
    "110SynapseRadius", 1.0, 0.1, 5.0, {"Synapse radius"}};
const brayns::Property PROP_LOAD_AFFERENT_SYNAPSES = {
    "111LoadAfferentSynapses", false, {"Loads afferent synapses"}};
const brayns::Property PROP_LOAD_EFFERENT_SYNAPSES = {
    "112LoadEfferentSynapses", false, {"Loads efferent synapses"}};
const brayns::Property PROP_LOAD_LAYERS = {
    "120LoadLayers", true,
    { "Load circuit layer data for coloring"}};
const brayns::Property PROP_LOAD_ETYPES = {
    "121LoadEtypes", true,
    { "Load circuit e-types for coloring"}};
const brayns::Property PROP_LOAD_MTYPES = {
    "122LoadMtypes", true,
    { "Load circuit m-types for coloring"}};
// clang-format on

struct MorphologyInfo
{
    brayns::Vector3d somaPosition;
    brayns::Boxd bounds;
    float maxDistanceToSoma;
};

struct ParallelModelContainer
{
    void addSphere(const size_t materialId, const brayns::Sphere& sphere)
    {
        spheres[materialId].push_back(sphere);
    }

    void addCylinder(const size_t materialId, const brayns::Cylinder& cylinder)
    {
        cylinders[materialId].push_back(cylinder);
    }

    void addCone(const size_t materialId, const brayns::Cone& cone)
    {
        cones[materialId].push_back(cone);
    }

    void addSDFGeometry(const size_t materialId,
                        const brayns::SDFGeometry& geom,
                        const std::vector<size_t> neighbours)
    {
        sdfMaterials.push_back(materialId);
        sdfGeometries.push_back(geom);
        sdfNeighbours.push_back(neighbours);
    }

    void addSpheresToModel(brayns::Model& model) const
    {
        for (const auto& sphere : spheres)
        {
            const auto index = sphere.first;
            model.getSpheres()[index].insert(model.getSpheres()[index].end(),
                                             sphere.second.begin(),
                                             sphere.second.end());
        }
    }

    void addCylindersToModel(brayns::Model& model) const
    {
        for (const auto& cylinder : cylinders)
        {
            const auto index = cylinder.first;
            model.getCylinders()[index].insert(
                model.getCylinders()[index].end(), cylinder.second.begin(),
                cylinder.second.end());
        }
    }

    void addConesToModel(brayns::Model& model) const
    {
        for (const auto& cone : cones)
        {
            const auto index = cone.first;
            model.getCones()[index].insert(model.getCones()[index].end(),
                                           cone.second.begin(),
                                           cone.second.end());
        }
    }

    void addSDFGeometriesToModel(brayns::Model& model) const
    {
        const size_t numGeoms = sdfGeometries.size();
        std::vector<size_t> localToGlobalIndex(numGeoms, 0);

        // Add geometries to Model. We do not know the indices of the neighbours
        // yet so we leave them empty.
        for (size_t i = 0; i < numGeoms; i++)
            localToGlobalIndex[i] =
                model.addSDFGeometry(sdfMaterials[i], sdfGeometries[i], {});

        // Write the neighbours using global indices
        std::vector<size_t> neighboursTmp;
        for (size_t i = 0; i < numGeoms; i++)
        {
            const size_t globalIndex = localToGlobalIndex[i];
            neighboursTmp.clear();

            for (auto localNeighbourIndex : sdfNeighbours[i])
                neighboursTmp.push_back(
                    localToGlobalIndex[localNeighbourIndex]);

            model.updateSDFGeometryNeighbours(globalIndex, neighboursTmp);
        }
    }

    void applyTransformation(const brayns::Matrix4f& transformation)
    {
        glm::vec3 scale;
        glm::quat rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(transformation, scale, rotation, translation, skew,
                       perspective);

        for (auto& s : spheres)
            for (auto& sphere : s.second)
                sphere.center = translation + rotation * sphere.center;
        for (auto& c : cylinders)
            for (auto& cylinder : c.second)
            {
                cylinder.center = translation + rotation * cylinder.center;
                cylinder.up = translation + rotation * cylinder.up;
            }
        for (auto& c : cones)
            for (auto& cone : c.second)
            {
                cone.center = translation + rotation * cone.center;
                cone.up = translation + rotation * cone.up;
            }
        for (auto& s : sdfGeometries)
        {
            s.p0 = translation + rotation * s.p0;
            s.p1 = translation + rotation * s.p1;
        }
    }

    brayns::SpheresMap spheres;
    brayns::CylindersMap cylinders;
    brayns::ConesMap cones;
    brayns::TriangleMeshMap trianglesMeshes;
    MorphologyInfo morphologyInfo;
    std::vector<brayns::SDFGeometry> sdfGeometries;
    std::vector<std::vector<size_t>> sdfNeighbours;
    std::vector<size_t> sdfMaterials;
};

#endif // CIRCUIT_EXPLORER_TYPES_H
