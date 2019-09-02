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

#include "MeshCircuitLoader.h"

#include <common/log.h>

const std::string LOADER_NAME = "Circuit viewer with meshes use-case";
const double DEFAULT_RADIUS_MULTIPLIER = 2.0;

MeshCircuitLoader::MeshCircuitLoader(
    brayns::Scene &scene,
    const brayns::ApplicationParameters &applicationParameters,
    brayns::PropertyMap &&loaderParams)
    : AbstractCircuitLoader(scene, applicationParameters,
                            std::move(loaderParams))
{
    PLUGIN_INFO << "Registering " << LOADER_NAME << std::endl;
    _fixedDefaults.setProperty(
        {PROP_DB_CONNECTION_STRING.name, std::string("")});
    _fixedDefaults.setProperty({PROP_USE_SDF_GEOMETRY.name, false});
    _fixedDefaults.setProperty(
        {PROP_PRESYNAPTIC_NEURON_GID.name, std::string("")});
    _fixedDefaults.setProperty(
        {PROP_POSTSYNAPTIC_NEURON_GID.name, std::string("")});
    _fixedDefaults.setProperty(
        {PROP_REPORT_TYPE.name, enumToString(ReportType::voltages_from_file)});
    _fixedDefaults.setProperty({PROP_CIRCUIT_COLOR_SCHEME.name,
                                enumToString(CircuitColorScheme::by_id)});
    _fixedDefaults.setProperty(
        {PROP_RADIUS_MULTIPLIER.name, DEFAULT_RADIUS_MULTIPLIER});
    _fixedDefaults.setProperty({PROP_RADIUS_CORRECTION.name, 0.0});
    _fixedDefaults.setProperty({PROP_USE_SDF_GEOMETRY.name, false});
    _fixedDefaults.setProperty(
        {PROP_DAMPEN_BRANCH_THICKNESS_CHANGERATE.name, false});
    _fixedDefaults.setProperty({PROP_USE_REALISTIC_SOMA.name, false});
    _fixedDefaults.setProperty({PROP_METABALLS_SAMPLES_FROM_SOMA.name, 0});
    _fixedDefaults.setProperty({PROP_METABALLS_GRID_SIZE.name, 0});
    _fixedDefaults.setProperty({PROP_METABALLS_THRESHOLD.name, 0.0});
    _fixedDefaults.setProperty({PROP_USER_DATA_TYPE.name,
                                enumToString(UserDataType::simulation_offset)});
    _fixedDefaults.setProperty({PROP_MORPHOLOGY_COLOR_SCHEME.name,
                                enumToString(MorphologyColorScheme::none)});
    _fixedDefaults.setProperty(
        {PROP_MORPHOLOGY_QUALITY.name, enumToString(MorphologyQuality::high)});
    _fixedDefaults.setProperty({PROP_MORPHOLOGY_MAX_DISTANCE_TO_SOMA.name,
                                std::numeric_limits<double>::max()});
    _fixedDefaults.setProperty({PROP_CELL_CLIPPING.name, false});
    _fixedDefaults.setProperty({PROP_AREAS_OF_INTEREST.name, 0});
    _fixedDefaults.setProperty({PROP_SYNAPSE_RADIUS.name, 1.0});
    _fixedDefaults.setProperty({PROP_LOAD_AFFERENT_SYNAPSES.name, false});
    _fixedDefaults.setProperty({PROP_LOAD_EFFERENT_SYNAPSES.name, false});
}

brayns::ModelDescriptorPtr MeshCircuitLoader::importFromFile(
    const std::string &filename, const brayns::LoaderProgress &callback,
    const brayns::PropertyMap &properties) const
{
    PLUGIN_INFO << "Loading circuit from " << filename << std::endl;
    callback.updateProgress("Loading circuit ...", 0);
    brayns::PropertyMap props = _defaults;
    props.merge(_fixedDefaults);
    props.merge(properties);
    return importCircuit(filename, props, callback);
}

std::string MeshCircuitLoader::getName() const
{
    return LOADER_NAME;
}

brayns::PropertyMap MeshCircuitLoader::getCLIProperties()
{
    brayns::PropertyMap pm("MeshCircuitExplorer");
    pm.setProperty(PROP_DENSITY);
    pm.setProperty(PROP_REPORT);
    pm.setProperty(PROP_SYNCHRONOUS_MODE);
    pm.setProperty(PROP_TARGETS);
    pm.setProperty(PROP_GIDS);
    pm.setProperty(PROP_RANDOM_SEED);
    pm.setProperty(PROP_MESH_FOLDER);
    pm.setProperty(PROP_MESH_FILENAME_PATTERN);
    pm.setProperty(PROP_MESH_TRANSFORMATION);
    pm.setProperty(PROP_SECTION_TYPE_SOMA);
    pm.setProperty(PROP_SECTION_TYPE_AXON);
    pm.setProperty(PROP_SECTION_TYPE_DENDRITE);
    pm.setProperty(PROP_SECTION_TYPE_APICAL_DENDRITE);
    return pm;
}
