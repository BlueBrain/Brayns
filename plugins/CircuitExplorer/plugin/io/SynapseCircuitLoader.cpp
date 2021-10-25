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

#include "SynapseCircuitLoader.h"

#include <brayns/common/Log.h>

const std::string LOADER_NAME = "Synapse loader use-case";

SynapseCircuitLoader::SynapseCircuitLoader(
    brayns::Scene &scene,
    const brayns::ApplicationParameters &applicationParameters,
    brayns::PropertyMap &&loaderParams, CircuitExplorerPlugin *plugin)
    : AbstractCircuitLoader(scene, applicationParameters,
                            std::move(loaderParams), plugin)
{
    brayns::Log::info("[CE] Registering {}.", LOADER_NAME);
    _fixedDefaults.add({PROP_DB_CONNECTION_STRING.getName(), std::string("")});
    _fixedDefaults.add({PROP_REPORT.getName(), std::string("")});
    _fixedDefaults.add(
        {PROP_PRESYNAPTIC_NEURON_GID.getName(), std::string("")});
    _fixedDefaults.add(
        {PROP_POSTSYNAPTIC_NEURON_GID.getName(), std::string("")});
    _fixedDefaults.add(
        {PROP_REPORT_TYPE.getName(), enumToString(ReportType::undefined)});
    _fixedDefaults.add({PROP_RADIUS_CORRECTION.getName(), 0.0});
    _fixedDefaults.add(
        {PROP_DAMPEN_BRANCH_THICKNESS_CHANGERATE.getName(), true});
    _fixedDefaults.add({PROP_USE_REALISTIC_SOMA.getName(), false});
    _fixedDefaults.add({PROP_METABALLS_SAMPLES_FROM_SOMA.getName(), 0});
    _fixedDefaults.add({PROP_METABALLS_GRID_SIZE.getName(), 0});
    _fixedDefaults.add({PROP_METABALLS_THRESHOLD.getName(), 0.0});
    _fixedDefaults.add(
        {PROP_USER_DATA_TYPE.getName(), enumToString(UserDataType::undefined)});
    _fixedDefaults.add({PROP_MORPHOLOGY_MAX_DISTANCE_TO_SOMA.getName(),
                        std::numeric_limits<double>::max()});
    _fixedDefaults.add({PROP_MESH_FOLDER.getName(), std::string("")});
    _fixedDefaults.add({PROP_MESH_FILENAME_PATTERN.getName(), std::string("")});
    _fixedDefaults.add({PROP_MESH_TRANSFORMATION.getName(), false});
    _fixedDefaults.add({PROP_CELL_CLIPPING.getName(), false});
    _fixedDefaults.add({PROP_AREAS_OF_INTEREST.getName(), 0});
}

std::vector<brayns::ModelDescriptorPtr> SynapseCircuitLoader::importFromFile(
    const std::string &filename, const brayns::LoaderProgress &callback,
    const brayns::PropertyMap &properties) const
{
    brayns::Log::info("[CE] Loading circuit from {}.", filename);
    callback.updateProgress("Loading circuit ...", 0);
    brayns::PropertyMap props = _defaults;
    props.merge(_fixedDefaults);
    props.merge(properties);
    return {importCircuit(filename, props, callback)};
}

std::string SynapseCircuitLoader::getName() const
{
    return LOADER_NAME;
}

brayns::PropertyMap SynapseCircuitLoader::getCLIProperties()
{
    brayns::PropertyMap pm("Synapse Circuit Loader");
    pm.add(PROP_DENSITY);
    pm.add(PROP_TARGETS);
    pm.add(PROP_GIDS);
    pm.add(PROP_RADIUS_MULTIPLIER);
    pm.add(PROP_RANDOM_SEED);
    pm.add(PROP_CIRCUIT_COLOR_SCHEME);
    pm.add(PROP_SECTION_TYPE_SOMA);
    pm.add(PROP_SECTION_TYPE_AXON);
    pm.add(PROP_SECTION_TYPE_DENDRITE);
    pm.add(PROP_SECTION_TYPE_APICAL_DENDRITE);
    pm.add(PROP_USE_SDF_GEOMETRY);
    pm.add(PROP_MORPHOLOGY_COLOR_SCHEME);
    pm.add(PROP_MORPHOLOGY_QUALITY);
    pm.add(PROP_SYNAPSE_RADIUS);
    pm.add(PROP_LOAD_AFFERENT_SYNAPSES);
    pm.add(PROP_LOAD_EFFERENT_SYNAPSES);
    return pm;
}
