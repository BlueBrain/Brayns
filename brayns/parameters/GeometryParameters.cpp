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
#include <brayns/common/exceptions.h>
#include <brayns/common/log.h>
#include <brayns/common/types.h>

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

namespace
{
const std::string PARAM_MORPHOLOGY_FOLDER = "morphology-folder";
const std::string PARAM_NEST_CIRCUIT = "nest-circuit";
const std::string PARAM_NEST_REPORT = "nest-report";
const std::string PARAM_PDB_FILE = "pdb-file";
const std::string PARAM_PDB_FOLDER = "pdb-folder";
const std::string PARAM_XYZB_FILE = "xyzb-file";
const std::string PARAM_MESH_FOLDER = "mesh-folder";
const std::string PARAM_MESH_FILE = "mesh-file";
const std::string PARAM_CIRCUIT_CONFIG = "circuit-config";
const std::string PARAM_CIRCUIT_DENSITY = "circuit-density";
const std::string PARAM_CIRCUIT_USES_SIMULATION_MODEL =
    "circuit-uses-simulation-model";
const std::string PARAM_CIRCUIT_BOUNDING_BOX = "circuit-bounding-box";
const std::string PARAM_CIRCUIT_MESH_FOLDER = "circuit-mesh-folder";
const std::string PARAM_CIRCUIT_MESH_FILENAME_PATTERN =
    "circuit-mesh-filename-pattern";
const std::string PARAM_CIRCUIT_MESH_TRANSFORMATION =
    "circuit-mesh-transformation";
const std::string PARAM_CIRCUIT_TARGETS = "circuit-targets";
const std::string PARAM_CIRCUIT_REPORT = "circuit-report";
const std::string PARAM_CIRCUIT_START_SIMULATION_TIME =
    "circuit-start-simulation-time";
const std::string PARAM_CIRCUIT_END_SIMULATION_TIME =
    "circuit-end-simulation-time";
const std::string PARAM_CIRCUIT_SIMULATION_STEP = "circuit-simulation-step";
const std::string PARAM_CIRCUIT_SIMULATION_RANGE =
    "circuit-simulation-values-range";
const std::string PARAM_CIRCUIT_SIMULATION_HISTOGRAM_SIZE =
    "circuit-simulation-histogram-size";
const std::string PARAM_LOAD_CACHE_FILE = "load-cache-file";
const std::string PARAM_SAVE_CACHE_FILE = "save-cache-file";
const std::string PARAM_RADIUS_MULTIPLIER = "radius-multiplier";
const std::string PARAM_RADIUS_CORRECTION = "radius-correction";
const std::string PARAM_COLOR_SCHEME = "color-scheme";
const std::string PARAM_SCENE_ENVIRONMENT = "scene-environment";
const std::string PARAM_GEOMETRY_QUALITY = "geometry-quality";
const std::string PARAM_NEST_CACHE_FILENAME = "nest-cache-file";
const std::string PARAM_MORPHOLOGY_SECTION_TYPES = "morphology-section-types";
const std::string PARAM_MORPHOLOGY_LAYOUT = "morphology-layout";
const std::string PARAM_GENERATE_MULTIPLE_MODELS = "generate-multiple-models";
const std::string PARAM_SPLASH_SCENE_FOLDER = "splash-scene-folder";
const std::string PARAM_MOLECULAR_SYSTEM_CONFIG = "molecular-system-config";
const std::string PARAM_METABALLS_GRIDSIZE = "metaballs-grid-size";
const std::string PARAM_METABALLS_THRESHOLD = "metaballs-threshold";
const std::string PARAM_METABALLS_SAMPLES_FROM_SOMA =
    "metaballs-samples-from-soma";
const std::string PARAM_MEMORY_MODE = "memory-mode";
const std::string PARAM_SCENE_FILE = "scene-file";

const std::string COLOR_SCHEMES[12] = {"none",
                                       "neuron-by-id",
                                       "neuron-by-type",
                                       "neuron-by-segment-type",
                                       "neuron-by-layer",
                                       "neuron-by-mtype",
                                       "neuron-by-etype",
                                       "neuron-by-target",
                                       "protein-by-id",
                                       "protein-atoms",
                                       "protein-chains",
                                       "protein-residues"};

const std::string SCENE_ENVIRONMENTS[4] = {"none", "ground", "wall",
                                           "bounding-box"};

const std::string GEOMETRY_QUALITIES[3] = {"low", "medium", "high"};
const std::string GEOMETRY_MEMORY_MODES[2] = {"shared", "replicated"};
}

namespace brayns
{
GeometryParameters::GeometryParameters()
    : AbstractParameters("Geometry")
    , _circuitUseSimulationModel(false)
    , _circuitBoundingBox(Vector3f(0.f), Vector3f(0.f))
    , _circuitDensity(100)
    , _circuitStartSimulationTime(0.f)
    , _circuitEndSimulationTime(std::numeric_limits<float>::max())
    , _circuitSimulationValuesRange(Vector2f(std::numeric_limits<float>::max(),
                                             std::numeric_limits<float>::min()))
    , _circuitSimulationHistogramSize(128)
    , _circuitMeshTransformation(false)
    , _sceneEnvironment(SceneEnvironment::none)
    , _radiusMultiplier(1.f)
    , _radiusCorrection(0.f)
    , _colorScheme(ColorScheme::none)
    , _geometryQuality(GeometryQuality::high)
    , _morphologySectionTypes(size_t(MorphologySectionType::all))
    , _generateMultipleModels(false)
    , _metaballsGridSize(0)
    , _metaballsThreshold(1.f)
    , _metaballsSamplesFromSoma(3)
    , _memoryMode(MemoryMode::shared)
{
    _parameters.add_options()(PARAM_MORPHOLOGY_FOLDER.c_str(),
                              po::value<std::string>(),
                              "Folder containing SWC and H5 files [string]")(
        PARAM_NEST_CIRCUIT.c_str(), po::value<std::string>(),
        "H5 file containing the NEST circuit [string]")(
        PARAM_NEST_REPORT.c_str(), po::value<std::string>(),
        "NEST simulation report file [string]")(
        PARAM_MESH_FOLDER.c_str(), po::value<std::string>(),
        "Folder containing mesh files [string]")(PARAM_MESH_FILE.c_str(),
                                                 po::value<std::string>(),
                                                 "Mesh file [string]")(
        PARAM_PDB_FILE.c_str(), po::value<std::string>(),
        "PDB filename [string]")(PARAM_PDB_FOLDER.c_str(),
                                 po::value<std::string>(),
                                 "Folder containing PDB files [string]")(
        PARAM_XYZB_FILE.c_str(), po::value<std::string>(),
        "XYZB filename [string]")(PARAM_CIRCUIT_CONFIG.c_str(),
                                  po::value<std::string>(),
                                  "Circuit configuration filename [string]")(
        PARAM_LOAD_CACHE_FILE.c_str(), po::value<std::string>(),
        "Load binary container of a scene [string]")(
        PARAM_SAVE_CACHE_FILE.c_str(), po::value<std::string>(),
        "Save binary container of a scene [string]")(
        PARAM_RADIUS_MULTIPLIER.c_str(), po::value<float>(),
        "Radius multiplier for spheres, cones and cylinders [float]")(
        PARAM_RADIUS_CORRECTION.c_str(), po::value<float>(),
        "Forces radius of spheres and cylinders to the specified value "
        "[float]")(
        PARAM_COLOR_SCHEME.c_str(), po::value<std::string>(),
        "Color scheme to be applied to the geometry [none|"
        "neuron_by_id|neuron_by_type|neuron_by_segment_type|"
        "protein_atoms|protein_chains|protein_residues|protein_backbones]")(
        PARAM_SCENE_ENVIRONMENT.c_str(), po::value<std::string>(),
        "Scene environment [none|ground|wall|bounding-box]")(
        PARAM_GEOMETRY_QUALITY.c_str(), po::value<std::string>(),
        "Geometry rendering quality [low|medium|high]")(
        PARAM_CIRCUIT_TARGETS.c_str(), po::value<std::string>(),
        "Circuit targets [comma separated strings]")(
        PARAM_CIRCUIT_DENSITY.c_str(), po::value<float>(),
        "Density of cells in the circuit in percent [float]")(
        PARAM_CIRCUIT_MESH_FOLDER.c_str(), po::value<std::string>(),
        "Folder containing meshed morphologies [string]")(
        PARAM_CIRCUIT_REPORT.c_str(), po::value<std::string>(),
        "Circuit report [string]")(
        PARAM_MORPHOLOGY_SECTION_TYPES.c_str(), po::value<size_t>(),
        "Morphology section types (1: soma, 2: axon, 4: dendrite, "
        "8: apical dendrite). Values can be added to select more than "
        "one type of section")(PARAM_MORPHOLOGY_LAYOUT.c_str(),
                               po::value<size_ts>()->multitoken(),
                               "Morphology layout defined by number of "
                               "columns, vertical spacing, horizontal spacing "
                               "[int int int]")(
        PARAM_CIRCUIT_START_SIMULATION_TIME.c_str(), po::value<double>(),
        "Start simulation timestamp [double]")(
        PARAM_CIRCUIT_END_SIMULATION_TIME.c_str(), po::value<double>(),
        "End simulation timestamp [double]")(
        PARAM_CIRCUIT_SIMULATION_STEP.c_str(), po::value<double>(),
        "Step between simulation frames [double]")(
        PARAM_CIRCUIT_SIMULATION_RANGE.c_str(),
        po::value<floats>()->multitoken(),
        "Minimum and maximum values for the simulation [float float]")(
        PARAM_CIRCUIT_SIMULATION_HISTOGRAM_SIZE.c_str(), po::value<size_t>(),
        "Number of values defining the simulation histogram [int]")(
        PARAM_NEST_CACHE_FILENAME.c_str(), po::value<std::string>(),
        "Cache file containing nest data [string]")(
        PARAM_GENERATE_MULTIPLE_MODELS.c_str(), po::value<bool>(),
        "Enable/Disable generation of multiple models based on geometry "
        "timestamps [bool]")(PARAM_SPLASH_SCENE_FOLDER.c_str(),
                             po::value<std::string>(),
                             "Folder containing splash scene folder [string]")(
        PARAM_MOLECULAR_SYSTEM_CONFIG.c_str(), po::value<std::string>(),
        "Molecular system configuration [string]")(
        PARAM_METABALLS_GRIDSIZE.c_str(), po::value<size_t>(),
        "Metaballs grid size [int]. Activates automated meshing of somas "
        "if different from 0")(PARAM_METABALLS_THRESHOLD.c_str(),
                               po::value<float>(),
                               "Metaballs threshold [float]")(
        PARAM_METABALLS_SAMPLES_FROM_SOMA.c_str(), po::value<size_t>(),
        "Number of morphology samples (or segments) from soma used by "
        "automated meshing [int]")(PARAM_CIRCUIT_USES_SIMULATION_MODEL.c_str(),
                                   po::value<bool>(),
                                   "Defines if a different model is used to "
                                   "handle the simulation geometry [bool]")(
        PARAM_CIRCUIT_BOUNDING_BOX.c_str(), po::value<floats>()->multitoken(),
        "Does not load circuit geometry outside of the specified bounding box"
        "[float float float float float float]")(
        PARAM_MEMORY_MODE.c_str(), po::value<std::string>(),
        "Defines what memory mode should be used between Brayns and the "
        "underlying renderer [shared|replicated]")(
        PARAM_SCENE_FILE.c_str(), po::value<std::string>(),
        "Full path of a file containing a scene description [string]")(
        PARAM_CIRCUIT_MESH_FILENAME_PATTERN.c_str(), po::value<std::string>(),
        "Pattern used to determine the name of the file containing a meshed "
        "morphology [string]")(PARAM_CIRCUIT_MESH_TRANSFORMATION.c_str(),
                               po::value<bool>(),
                               "Enable/Disable mesh transformation according "
                               "to circuit information [bool]");
}

bool GeometryParameters::_parse(const po::variables_map& vm)
{
    if (vm.count(PARAM_MORPHOLOGY_FOLDER))
        _morphologyFolder = vm[PARAM_MORPHOLOGY_FOLDER].as<std::string>();
    if (vm.count(PARAM_NEST_CIRCUIT))
        _NESTCircuit = vm[PARAM_NEST_CIRCUIT].as<std::string>();
    if (vm.count(PARAM_NEST_REPORT))
        _NESTReport = vm[PARAM_NEST_REPORT].as<std::string>();
    if (vm.count(PARAM_PDB_FILE))
        _pdbFile = vm[PARAM_PDB_FILE].as<std::string>();
    if (vm.count(PARAM_PDB_FOLDER))
        _pdbFolder = vm[PARAM_PDB_FOLDER].as<std::string>();
    if (vm.count(PARAM_XYZB_FILE))
        _xyzbFile = vm[PARAM_XYZB_FILE].as<std::string>();
    if (vm.count(PARAM_MESH_FOLDER))
        _meshFolder = vm[PARAM_MESH_FOLDER].as<std::string>();
    if (vm.count(PARAM_MESH_FILE))
        _meshFile = vm[PARAM_MESH_FILE].as<std::string>();
    if (vm.count(PARAM_CIRCUIT_CONFIG))
        _circuitConfiguration = vm[PARAM_CIRCUIT_CONFIG].as<std::string>();
    if (vm.count(PARAM_LOAD_CACHE_FILE))
        _loadCacheFile = vm[PARAM_LOAD_CACHE_FILE].as<std::string>();
    if (vm.count(PARAM_SAVE_CACHE_FILE))
        _saveCacheFile = vm[PARAM_SAVE_CACHE_FILE].as<std::string>();
    if (vm.count(PARAM_COLOR_SCHEME))
    {
        _colorScheme = ColorScheme::none;
        const auto& colorScheme = vm[PARAM_COLOR_SCHEME].as<std::string>();
        for (size_t i = 0; i < sizeof(COLOR_SCHEMES) / sizeof(COLOR_SCHEMES[0]);
             ++i)
            if (colorScheme == COLOR_SCHEMES[i])
                _colorScheme = static_cast<ColorScheme>(i);
    }
    if (vm.count(PARAM_RADIUS_MULTIPLIER))
        _radiusMultiplier = vm[PARAM_RADIUS_MULTIPLIER].as<float>();
    if (vm.count(PARAM_RADIUS_CORRECTION))
        _radiusCorrection = vm[PARAM_RADIUS_CORRECTION].as<float>();
    if (vm.count(PARAM_SCENE_ENVIRONMENT))
    {
        _sceneEnvironment = SceneEnvironment::none;
        const auto& sceneEnvironment =
            vm[PARAM_SCENE_ENVIRONMENT].as<std::string>();
        for (size_t i = 0;
             i < sizeof(SCENE_ENVIRONMENTS) / sizeof(SCENE_ENVIRONMENTS[0]);
             ++i)
            if (sceneEnvironment == SCENE_ENVIRONMENTS[i])
                _sceneEnvironment = static_cast<SceneEnvironment>(i);
    }
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
        _circuitTargets = vm[PARAM_CIRCUIT_TARGETS].as<std::string>();
    if (vm.count(PARAM_CIRCUIT_REPORT))
        _circuitReport = vm[PARAM_CIRCUIT_REPORT].as<std::string>();
    if (vm.count(PARAM_CIRCUIT_DENSITY))
        _circuitDensity = vm[PARAM_CIRCUIT_DENSITY].as<float>();
    if (vm.count(PARAM_CIRCUIT_MESH_FOLDER))
        _circuitMeshFolder = vm[PARAM_CIRCUIT_MESH_FOLDER].as<std::string>();
    if (vm.count(PARAM_MORPHOLOGY_SECTION_TYPES))
        _morphologySectionTypes =
            vm[PARAM_MORPHOLOGY_SECTION_TYPES].as<size_t>();
    if (vm.count(PARAM_MORPHOLOGY_LAYOUT))
    {
        size_ts values = vm[PARAM_MORPHOLOGY_LAYOUT].as<size_ts>();
        if (values.size() == 3)
        {
            _morphologyLayout.nbColumns = values[0];
            _morphologyLayout.verticalSpacing = values[1];
            _morphologyLayout.horizontalSpacing = values[2];
        }
    }
    if (vm.count(PARAM_CIRCUIT_START_SIMULATION_TIME))
        _circuitStartSimulationTime =
            vm[PARAM_CIRCUIT_START_SIMULATION_TIME].as<double>();
    if (vm.count(PARAM_CIRCUIT_END_SIMULATION_TIME))
        _circuitEndSimulationTime =
            vm[PARAM_CIRCUIT_END_SIMULATION_TIME].as<double>();
    if (vm.count(PARAM_CIRCUIT_SIMULATION_STEP))
        _circuitSimulationStep = vm[PARAM_CIRCUIT_SIMULATION_STEP].as<double>();
    if (vm.count(PARAM_CIRCUIT_SIMULATION_RANGE))
    {
        floats values = vm[PARAM_CIRCUIT_SIMULATION_RANGE].as<floats>();
        if (values.size() == 2)
            _circuitSimulationValuesRange = Vector2f(values[0], values[1]);
    }
    if (vm.count(PARAM_CIRCUIT_SIMULATION_HISTOGRAM_SIZE))
        _circuitSimulationHistogramSize =
            vm[PARAM_CIRCUIT_SIMULATION_HISTOGRAM_SIZE].as<size_t>();
    if (vm.count(PARAM_NEST_CACHE_FILENAME))
        _NESTCacheFile = vm[PARAM_NEST_CACHE_FILENAME].as<std::string>();
    if (vm.count(PARAM_GENERATE_MULTIPLE_MODELS))
        _generateMultipleModels = vm[PARAM_GENERATE_MULTIPLE_MODELS].as<bool>();
    if (vm.count(PARAM_SPLASH_SCENE_FOLDER))
        _splashSceneFolder = vm[PARAM_SPLASH_SCENE_FOLDER].as<std::string>();
    if (vm.count(PARAM_MOLECULAR_SYSTEM_CONFIG))
        _molecularSystemConfig =
            vm[PARAM_MOLECULAR_SYSTEM_CONFIG].as<std::string>();

    if (vm.count(PARAM_METABALLS_GRIDSIZE))
        _metaballsGridSize = vm[PARAM_METABALLS_GRIDSIZE].as<size_t>();
    if (vm.count(PARAM_METABALLS_THRESHOLD))
        _metaballsThreshold = vm[PARAM_METABALLS_THRESHOLD].as<float>();
    if (vm.count(PARAM_METABALLS_SAMPLES_FROM_SOMA))
        _metaballsSamplesFromSoma =
            vm[PARAM_METABALLS_SAMPLES_FROM_SOMA].as<size_t>();
    if (vm.count(PARAM_CIRCUIT_USES_SIMULATION_MODEL))
        _circuitUseSimulationModel =
            vm[PARAM_CIRCUIT_USES_SIMULATION_MODEL].as<bool>();
    if (vm.count(PARAM_CIRCUIT_BOUNDING_BOX))
    {
        const floats values = vm[PARAM_CIRCUIT_BOUNDING_BOX].as<floats>();
        if (values.size() == 6)
        {
            _circuitBoundingBox.reset();
            _circuitBoundingBox.merge(
                Vector3f(values[0], values[1], values[2]));
            _circuitBoundingBox.merge(
                Vector3f(values[3], values[4], values[5]));
            BRAYNS_ERROR << _circuitBoundingBox << std::endl;
        }
        else
            BRAYNS_ERROR << "Invalid number of values for "
                         << PARAM_CIRCUIT_BOUNDING_BOX << std::endl;
    }
    if (vm.count(PARAM_MEMORY_MODE))
    {
        const auto& memoryMode = vm[PARAM_MEMORY_MODE].as<std::string>();
        for (size_t i = 0; i < sizeof(GEOMETRY_MEMORY_MODES) /
                                   sizeof(GEOMETRY_MEMORY_MODES[0]);
             ++i)
            if (memoryMode == GEOMETRY_MEMORY_MODES[i])
                _memoryMode = static_cast<MemoryMode>(i);
    }
    if (vm.count(PARAM_SCENE_FILE))
        _sceneFile = vm[PARAM_SCENE_FILE].as<std::string>();
    if (vm.count(PARAM_CIRCUIT_MESH_FILENAME_PATTERN))
        _circuitMeshFilenamePattern =
            vm[PARAM_CIRCUIT_MESH_FILENAME_PATTERN].as<std::string>();
    if (vm.count(PARAM_CIRCUIT_MESH_TRANSFORMATION))
        _circuitMeshTransformation =
            vm[PARAM_CIRCUIT_MESH_TRANSFORMATION].as<bool>();

    return true;
}

void GeometryParameters::print()
{
    AbstractParameters::print();
    BRAYNS_INFO << "Morphology folder          : " << _morphologyFolder
                << std::endl;
    BRAYNS_INFO << "NEST circuit file          : " << _NESTCircuit << std::endl;
    BRAYNS_INFO << "NEST simulation report file: " << _NESTReport << std::endl;
    BRAYNS_INFO << "NEST cache file            : " << _NESTCacheFile
                << std::endl;
    BRAYNS_INFO << "PDB file                   : " << _pdbFile << std::endl;
    BRAYNS_INFO << "PDB folder                 : " << _pdbFolder << std::endl;
    BRAYNS_INFO << "XYZB file                  : " << _xyzbFile << std::endl;
    BRAYNS_INFO << "Mesh folder                : " << _meshFolder << std::endl;
    BRAYNS_INFO << "Mesh file                  : " << _meshFile << std::endl;
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
    BRAYNS_INFO << "Scene environment          : "
                << getSceneEnvironmentAsString(_sceneEnvironment) << std::endl;
    BRAYNS_INFO << "Geometry quality           : "
                << getGeometryQualityAsString(_geometryQuality) << std::endl;
    BRAYNS_INFO << "Circuit configuration      : " << std::endl;
    BRAYNS_INFO << "- Config file              : " << _circuitConfiguration
                << std::endl;
    BRAYNS_INFO << "- Targets                  : " << _circuitTargets
                << std::endl;
    BRAYNS_INFO << "- Report                   : " << _circuitReport
                << std::endl;
    BRAYNS_INFO << "- Mesh folder              : " << _circuitMeshFolder
                << std::endl;
    BRAYNS_INFO << "- Density                  : " << _circuitDensity
                << std::endl;
    BRAYNS_INFO << "- Start simulation time    : "
                << _circuitStartSimulationTime << std::endl;
    BRAYNS_INFO << "- End simulation time      : " << _circuitEndSimulationTime
                << std::endl;
    BRAYNS_INFO << "- Simulation step          : " << _circuitSimulationStep
                << std::endl;
    BRAYNS_INFO << "- Simulation values range  : "
                << _circuitSimulationValuesRange << std::endl;
    BRAYNS_INFO << "- Simulation histogram size: "
                << _circuitSimulationHistogramSize << std::endl;
    BRAYNS_INFO << "- Bounding box             : " << _circuitBoundingBox
                << std::endl;
    BRAYNS_INFO << "- Mesh transformation      : "
                << (_circuitMeshTransformation ? "Yes" : "No") << std::endl;
    BRAYNS_INFO << "Morphology section types   : " << _morphologySectionTypes
                << std::endl;
    BRAYNS_INFO << "Morphology Layout          : " << std::endl;
    BRAYNS_INFO << " - Columns                 : "
                << _morphologyLayout.nbColumns << std::endl;
    BRAYNS_INFO << " - Vertical spacing        : "
                << _morphologyLayout.verticalSpacing << std::endl;
    BRAYNS_INFO << " - Horizontal spacing      : "
                << _morphologyLayout.horizontalSpacing << std::endl;
    BRAYNS_INFO << "Generate multiple models   : "
                << (_generateMultipleModels ? "Yes" : "No") << std::endl;
    BRAYNS_INFO << "Splash scene folder        : " << _splashSceneFolder
                << std::endl;
    BRAYNS_INFO << "Molecular system config    : " << _molecularSystemConfig
                << std::endl;
    BRAYNS_INFO << "Metaballs                  : " << std::endl;
    BRAYNS_INFO << " - Grid size               : " << _metaballsGridSize
                << std::endl;
    BRAYNS_INFO << " - Threshold               : " << _metaballsThreshold
                << std::endl;
    BRAYNS_INFO << " - Samples from soma       : " << _metaballsSamplesFromSoma
                << std::endl;
    BRAYNS_INFO << "Use simulation model       : "
                << (_circuitUseSimulationModel ? "Yes" : "No") << std::endl;
    BRAYNS_INFO << "Memory mode                : "
                << (_memoryMode == MemoryMode::shared ? "Shared" : "Replicated")
                << std::endl;
    BRAYNS_INFO << "Scene file                 : " << _sceneFile << std::endl;
    BRAYNS_INFO << "Mesh filename pattern      : "
                << _circuitMeshFilenamePattern << std::endl;
}

const std::string& GeometryParameters::getColorSchemeAsString(
    const ColorScheme value) const
{
    return COLOR_SCHEMES[static_cast<size_t>(value)];
}

const std::string& GeometryParameters::getSceneEnvironmentAsString(
    const SceneEnvironment value) const
{
    return SCENE_ENVIRONMENTS[static_cast<size_t>(value)];
}

const std::string& GeometryParameters::getGeometryQualityAsString(
    const GeometryQuality value) const
{
    return GEOMETRY_QUALITIES[static_cast<size_t>(value)];
}

float GeometryParameters::getCircuitDensity() const
{
    return std::max(0.f, std::min(100.f, _circuitDensity));
}

strings GeometryParameters::getCircuitTargetsAsStrings() const
{
    strings targets;
    boost::char_separator<char> separator(",");
    boost::tokenizer<boost::char_separator<char>> tokens(_circuitTargets,
                                                         separator);
    for_each(tokens.begin(), tokens.end(),
             [&targets](const std::string& s) { targets.push_back(s); });
    return targets;
}
}
