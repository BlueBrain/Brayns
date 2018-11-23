/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include "GeometryParameters.h"
#include <brayns/common/log.h>
#include <brayns/common/types.h>

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

namespace
{
const std::string PARAM_CIRCUIT_DENSITY = "circuit-density";
const std::string PARAM_CIRCUIT_USES_SIMULATION_MODEL =
    "circuit-uses-simulation-model";
const std::string PARAM_CIRCUIT_MESH_FOLDER = "circuit-mesh-folder";
const std::string PARAM_CIRCUIT_MESH_FILENAME_PATTERN =
    "circuit-mesh-filename-pattern";
const std::string PARAM_CIRCUIT_TRANSFORM_MESHES =
    "circuit-transform-meshes";
const std::string PARAM_CIRCUIT_TARGETS = "circuit-targets";
const std::string PARAM_CIRCUIT_REPORT = "circuit-report";
const std::string PARAM_CIRCUIT_START_SIMULATION_TIME =
    "circuit-start-simulation-time";
const std::string PARAM_CIRCUIT_END_SIMULATION_TIME =
    "circuit-end-simulation-time";
const std::string PARAM_CIRCUIT_SIMULATION_STEP = "circuit-simulation-step";
const std::string PARAM_CIRCUIT_SIMULATION_RANGE =
    "circuit-simulation-values-range";
const std::string PARAM_CIRCUIT_RANDOM_SEED = "circuit-random-seed";
const std::string PARAM_LOAD_CACHE_FILE = "load-cache-file";
const std::string PARAM_SAVE_CACHE_FILE = "save-cache-file";
const std::string PARAM_RADIUS_MULTIPLIER = "radius-multiplier";
const std::string PARAM_RADIUS_CORRECTION = "radius-correction";
const std::string PARAM_COLOR_SCHEME = "color-scheme";
const std::string PARAM_GEOMETRY_QUALITY = "geometry-quality";
const std::string PARAM_MORPHOLOGY_SECTION_TYPES = "morphology-section-types";
const std::string PARAM_MORPHOLOGY_DAMPEN_BRANCH_THICKNESS_CHANGERATE =
    "morphology-dampen-branch-thickness-changerate";
const std::string PARAM_MORPHOLOGY_USE_SDF_GEOMETRIES =
    "morphology-use-sdf-geometries";
const std::string PARAM_MEMORY_MODE = "memory-mode";
const std::string PARAM_DEFAULT_BVH_FLAG = "default-bvh-flag";

const std::array<std::string, 12> COLOR_SCHEMES = {
    {"none", "neuron-by-id", "neuron-by-type", "neuron-by-segment-type",
     "neuron-by-layer", "neuron-by-mtype", "neuron-by-etype",
     "neuron-by-target", "protein-by-id", "protein-atoms", "protein-chains",
     "protein-residues"}};

const std::string GEOMETRY_QUALITIES[3] = {"low", "medium", "high"};
const std::string GEOMETRY_MEMORY_MODES[2] = {"shared", "replicated"};
const std::map<std::string, brayns::BVHFlag> BVH_TYPES = {
    {"dynamic", brayns::BVHFlag::dynamic},
    {"compact", brayns::BVHFlag::compact},
    {"robust", brayns::BVHFlag::robust}};
}

namespace brayns
{
GeometryParameters::GeometryParameters()
    : AbstractParameters("Geometry")
    , _radiusMultiplier(1.f)
    , _radiusCorrection(0.f)
    , _colorScheme(ColorScheme::none)
    , _geometryQuality(GeometryQuality::high)
    , _morphologySectionTypes{MorphologySectionType::all}
    , _morphologyDampenBranchThicknessChangerate(false)
    , _morphologyUseSdfGeometries(false)
    , _memoryMode(MemoryMode::shared)
{
    _parameters.add_options() //
        (PARAM_LOAD_CACHE_FILE.c_str(), po::value<std::string>(),
         "Load binary container of a scene [string]")
        //
        (PARAM_SAVE_CACHE_FILE.c_str(), po::value<std::string>(),
         "Save binary container of a scene [string]")
        //
        (PARAM_RADIUS_MULTIPLIER.c_str(), po::value<float>(),
         "Radius multiplier for spheres, cones and cylinders [float]")
        //
        (PARAM_RADIUS_CORRECTION.c_str(), po::value<float>(),
         "Forces radius of spheres and cylinders to the specified value "
         "[float]")
        //
        (PARAM_COLOR_SCHEME.c_str(), po::value<std::string>(),
         "Color scheme to be applied to the geometry "
         "[none|neuron-by-id|neuron-by-type|neuron-by-segment-type|"
         "neuron-by-layer|neuron-by-mtype|neuron-by-etype|neuron-by-"
         "target|protein-by-id|protein-atoms|protein-chains|protein-"
         "residues]")
        //
        (PARAM_GEOMETRY_QUALITY.c_str(), po::value<std::string>(),
         "Geometry rendering quality [low|medium|high]")
        //
        (PARAM_CIRCUIT_TARGETS.c_str(), po::value<std::string>(),
         "Circuit targets [comma separated strings]")
        //
        (PARAM_CIRCUIT_DENSITY.c_str(), po::value<float>(),
         "Density of cells in the circuit in percent [float]")
        //
        (PARAM_CIRCUIT_MESH_FOLDER.c_str(), po::value<std::string>(),
         "Folder containing meshed morphologies [string]")
        //
        (PARAM_CIRCUIT_REPORT.c_str(), po::value<std::string>(),
         "Circuit report [string]")
        //
        (PARAM_MORPHOLOGY_SECTION_TYPES.c_str(), po::value<size_t>(),
         "Morphology section types (1: soma, 2: axon, 4: dendrite, "
         "8: apical dendrite). Values can be added to select more than "
         "one type of section")
        //
        (PARAM_CIRCUIT_START_SIMULATION_TIME.c_str(), po::value<double>(),
         "Start simulation timestamp [double]")
        //
        (PARAM_CIRCUIT_END_SIMULATION_TIME.c_str(), po::value<double>(),
         "End simulation timestamp [double]")
        //
        (PARAM_CIRCUIT_SIMULATION_STEP.c_str(), po::value<double>(),
         "Step between simulation frames [double]")
        //
        (PARAM_CIRCUIT_SIMULATION_RANGE.c_str(),
         po::value<floats>()->multitoken(),
         "Minimum and maximum values for the simulation [float float]")
        //
        (PARAM_CIRCUIT_RANDOM_SEED.c_str(), po::value<size_t>(),
         "Random seed for circuit [int]")
        //
        (PARAM_MORPHOLOGY_DAMPEN_BRANCH_THICKNESS_CHANGERATE.c_str(),
         po::bool_switch()->default_value(false),
         "Dampen the thickness rate of change for branches in the morphology.")
        //
        (PARAM_MORPHOLOGY_USE_SDF_GEOMETRIES.c_str(),
         po::bool_switch()->default_value(false),
         "Use SDF geometries for drawing the morphologies.")
        //
        (PARAM_CIRCUIT_USES_SIMULATION_MODEL.c_str(),
         po::bool_switch()->default_value(false),
         "Defines if a different model is used to handle the simulation "
         "geometry.")
        //
        (PARAM_MEMORY_MODE.c_str(), po::value<std::string>(),
         "Defines what memory mode should be used between Brayns and "
         "the "
         "underlying renderer [shared|replicated]")
        //
        (PARAM_CIRCUIT_MESH_FILENAME_PATTERN.c_str(), po::value<std::string>(),
         "Pattern used to determine the name of the file containing a "
         "meshed "
         "morphology [string]")
        //
        (PARAM_CIRCUIT_TRANSFORM_MESHES.c_str(),
         po::bool_switch()->default_value(false),
         "Enable mesh transformation according to circuit information.")
        //
        (PARAM_DEFAULT_BVH_FLAG.c_str(),
         po::value<std::vector<std::string>>()->multitoken(),
         "Set a default flag to apply to BVH creation, one of "
         "[dynamic|compact|robust], may appear multiple times.");
}

void GeometryParameters::parse(const po::variables_map& vm)
{
    if (vm.count(PARAM_LOAD_CACHE_FILE))
        _loadCacheFile = vm[PARAM_LOAD_CACHE_FILE].as<std::string>();
    if (vm.count(PARAM_SAVE_CACHE_FILE))
        _saveCacheFile = vm[PARAM_SAVE_CACHE_FILE].as<std::string>();
    if (vm.count(PARAM_COLOR_SCHEME))
    {
        _colorScheme = ColorScheme::none;
        const auto& colorScheme = vm[PARAM_COLOR_SCHEME].as<std::string>();
        if (!colorScheme.empty())
        {
            auto it = std::find(COLOR_SCHEMES.begin(), COLOR_SCHEMES.end(),
                                colorScheme);
            if (it == COLOR_SCHEMES.end())
                throw po::error("No match for color scheme '" + colorScheme);

            const auto index = std::distance(COLOR_SCHEMES.begin(), it);
            _colorScheme = static_cast<ColorScheme>(index);
        }
    }
    if (vm.count(PARAM_RADIUS_MULTIPLIER))
        _radiusMultiplier = vm[PARAM_RADIUS_MULTIPLIER].as<float>();
    if (vm.count(PARAM_RADIUS_CORRECTION))
        _radiusCorrection = vm[PARAM_RADIUS_CORRECTION].as<float>();
    if (vm.count(PARAM_GEOMETRY_QUALITY))
    {
        _geometryQuality = GeometryQuality::low;
        const auto& geometryQuality =
            vm[PARAM_GEOMETRY_QUALITY].as<std::string>();
        for (size_t i = 0;
             i < sizeof(GEOMETRY_QUALITIES) / sizeof(GEOMETRY_QUALITIES[0]);
             ++i)
            if (geometryQuality == GEOMETRY_QUALITIES[i])
                _geometryQuality = static_cast<GeometryQuality>(i);
    }
    if (vm.count(PARAM_CIRCUIT_TARGETS))
        _circuitConfiguration.targets =
            vm[PARAM_CIRCUIT_TARGETS].as<std::string>();
    if (vm.count(PARAM_CIRCUIT_REPORT))
        _circuitConfiguration.report =
            vm[PARAM_CIRCUIT_REPORT].as<std::string>();
    if (vm.count(PARAM_CIRCUIT_DENSITY))
        _circuitConfiguration.density = vm[PARAM_CIRCUIT_DENSITY].as<float>();
    if (vm.count(PARAM_CIRCUIT_MESH_FOLDER))
        _circuitConfiguration.meshFolder =
            vm[PARAM_CIRCUIT_MESH_FOLDER].as<std::string>();
    if (vm.count(PARAM_MORPHOLOGY_SECTION_TYPES))
    {
        _morphologySectionTypes.clear();
        const auto bits = vm[PARAM_MORPHOLOGY_SECTION_TYPES].as<size_t>();
        if (bits & size_t(MorphologySectionType::soma))
            _morphologySectionTypes.push_back(MorphologySectionType::soma);
        if (bits & size_t(MorphologySectionType::axon))
            _morphologySectionTypes.push_back(MorphologySectionType::axon);
        if (bits & size_t(MorphologySectionType::dendrite))
            _morphologySectionTypes.push_back(MorphologySectionType::dendrite);
        if (bits & size_t(MorphologySectionType::apical_dendrite))
            _morphologySectionTypes.push_back(
                MorphologySectionType::apical_dendrite);
    }
    if (vm.count(PARAM_CIRCUIT_START_SIMULATION_TIME))
        _circuitConfiguration.startSimulationTime =
            vm[PARAM_CIRCUIT_START_SIMULATION_TIME].as<double>();
    if (vm.count(PARAM_CIRCUIT_END_SIMULATION_TIME))
        _circuitConfiguration.endSimulationTime =
            vm[PARAM_CIRCUIT_END_SIMULATION_TIME].as<double>();
    if (vm.count(PARAM_CIRCUIT_SIMULATION_STEP))
        _circuitConfiguration.simulationStep =
            vm[PARAM_CIRCUIT_SIMULATION_STEP].as<double>();
    if (vm.count(PARAM_CIRCUIT_SIMULATION_RANGE))
    {
        floats values = vm[PARAM_CIRCUIT_SIMULATION_RANGE].as<floats>();
        if (values.size() == 2)
            _circuitConfiguration.simulationValuesRange =
                Vector2f(values[0], values[1]);
    }
    if (vm.count(PARAM_CIRCUIT_RANDOM_SEED))
        _circuitConfiguration.randomSeed =
            vm[PARAM_CIRCUIT_RANDOM_SEED].as<size_t>();

    _morphologyDampenBranchThicknessChangerate =
        vm[PARAM_MORPHOLOGY_DAMPEN_BRANCH_THICKNESS_CHANGERATE].as<bool>();
    _morphologyUseSdfGeometries =
        vm[PARAM_MORPHOLOGY_USE_SDF_GEOMETRIES].as<bool>();
    _circuitConfiguration.useSimulationModel =
        vm[PARAM_CIRCUIT_USES_SIMULATION_MODEL].as<bool>();
    if (vm.count(PARAM_MEMORY_MODE))
    {
        const auto& memoryMode = vm[PARAM_MEMORY_MODE].as<std::string>();
        for (size_t i = 0; i < sizeof(GEOMETRY_MEMORY_MODES) /
                                   sizeof(GEOMETRY_MEMORY_MODES[0]);
             ++i)
            if (memoryMode == GEOMETRY_MEMORY_MODES[i])
                _memoryMode = static_cast<MemoryMode>(i);
    }
    if (vm.count(PARAM_CIRCUIT_MESH_FILENAME_PATTERN))
        _circuitConfiguration.meshFilenamePattern =
            vm[PARAM_CIRCUIT_MESH_FILENAME_PATTERN].as<std::string>();
    _circuitConfiguration.transformMeshes =
        vm[PARAM_CIRCUIT_TRANSFORM_MESHES].as<bool>();

    if (vm.count(PARAM_DEFAULT_BVH_FLAG))
    {
        const auto& bvhs =
            vm[PARAM_DEFAULT_BVH_FLAG].as<std::vector<std::string>>();
        for (const auto& bvh : bvhs)
        {
            const auto kv = BVH_TYPES.find(bvh);
            if (kv != BVH_TYPES.end())
                _defaultBVHFlags.insert(kv->second);
            else
                throw std::runtime_error("Invalid bvh flag '" + bvh + "'.");
        }
    }

    markModified();
}

void GeometryParameters::print()
{
    AbstractParameters::print();
    BRAYNS_INFO << "Cache file to load         : " << _loadCacheFile
                << std::endl;
    BRAYNS_INFO << "Cache file to save         : " << _saveCacheFile
                << std::endl;
    BRAYNS_INFO << "Color scheme               : "
                << getColorSchemeAsString(_colorScheme) << std::endl;
    BRAYNS_INFO << "Radius multiplier          : " << _radiusMultiplier
                << std::endl;
    BRAYNS_INFO << "Radius correction          : " << _radiusCorrection
                << std::endl;
    BRAYNS_INFO << "Geometry quality           : "
                << getGeometryQualityAsString(_geometryQuality) << std::endl;
    BRAYNS_INFO << "Circuit configuration      : " << std::endl;
    BRAYNS_INFO << " - Targets                 : "
                << _circuitConfiguration.targets << std::endl;
    BRAYNS_INFO << " - Report                  : "
                << _circuitConfiguration.report << std::endl;
    BRAYNS_INFO << " - Mesh folder             : "
                << _circuitConfiguration.meshFolder << std::endl;
    BRAYNS_INFO << " - Density                 : "
                << _circuitConfiguration.density << std::endl;
    BRAYNS_INFO << " - Start simulation time   : "
                << _circuitConfiguration.startSimulationTime << std::endl;
    BRAYNS_INFO << " - End simulation time     : "
                << _circuitConfiguration.endSimulationTime << std::endl;
    BRAYNS_INFO << " - Simulation step         : "
                << _circuitConfiguration.simulationStep << std::endl;
    BRAYNS_INFO << " - Simulation values range : "
                << _circuitConfiguration.simulationValuesRange << std::endl;
    BRAYNS_INFO << " - Transform meshes        : "
                << (_circuitConfiguration.transformMeshes ? "Yes" : "No")
                << std::endl;
    BRAYNS_INFO << "Morphology section types   : "
                << enumsToBitmask(_morphologySectionTypes) << std::endl;
    BRAYNS_INFO << "Use simulation model       : "
                << (_circuitConfiguration.useSimulationModel ? "Yes" : "No")
                << std::endl;
    BRAYNS_INFO << "Memory mode                : "
                << (_memoryMode == MemoryMode::shared ? "Shared" : "Replicated")
                << std::endl;
    BRAYNS_INFO << "Mesh filename pattern      : "
                << _circuitConfiguration.meshFilenamePattern << std::endl;
}

const std::string& GeometryParameters::getColorSchemeAsString(
    const ColorScheme value) const
{
    return COLOR_SCHEMES[static_cast<size_t>(value)];
}
const std::string& GeometryParameters::getGeometryQualityAsString(
    const GeometryQuality value) const
{
    return GEOMETRY_QUALITIES[static_cast<size_t>(value)];
}

double GeometryParameters::getCircuitDensity() const
{
    return std::max(0., std::min(100., _circuitConfiguration.density));
}

strings GeometryParameters::getCircuitTargetsAsStrings() const
{
    strings targets;
    boost::char_separator<char> separator(",");
    boost::tokenizer<boost::char_separator<char>> tokens(
        _circuitConfiguration.targets, separator);
    for_each(tokens.begin(), tokens.end(),
             [&targets](const std::string& s) { targets.push_back(s); });
    return targets;
}
}
