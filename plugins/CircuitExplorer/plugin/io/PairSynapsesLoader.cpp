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

#include "PairSynapsesLoader.h"

#include <common/log.h>

const std::string LOADER_NAME = "Pair synapses use-case";

PairSynapsesLoader::PairSynapsesLoader(
    brayns::Scene &scene,
    const brayns::ApplicationParameters &applicationParameters,
    brayns::PropertyMap &&loaderParams, CircuitExplorerPlugin *plugin)
    : AbstractCircuitLoader(scene, applicationParameters,
                            std::move(loaderParams), plugin)
{
    PLUGIN_INFO << "Registering " << LOADER_NAME << std::endl;
    _fixedDefaults.add({PROP_DB_CONNECTION_STRING.getName(), std::string("")});
    _fixedDefaults.add({PROP_DENSITY.getName(), 1.0});
    _fixedDefaults.add({PROP_RANDOM_SEED.getName(), 0.0});
    _fixedDefaults.add({PROP_REPORT.getName(), std::string("")});
    _fixedDefaults.add({PROP_TARGETS.getName(), std::string("")});
    _fixedDefaults.add({PROP_GIDS.getName(), std::string("")});
    _fixedDefaults.add(
        {PROP_REPORT_TYPE.getName(), enumToString(ReportType::undefined)});
    _fixedDefaults.add({PROP_RADIUS_CORRECTION.getName(), 0.0});
    _fixedDefaults.add({PROP_CIRCUIT_COLOR_SCHEME.getName(),
                        enumToString(CircuitColorScheme::by_id)});
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
    _fixedDefaults.add({PROP_LOAD_AFFERENT_SYNAPSES.getName(), true});
    _fixedDefaults.add({PROP_LOAD_EFFERENT_SYNAPSES.getName(), true});
}

std::vector<brayns::ModelDescriptorPtr> PairSynapsesLoader::importFromFile(
    const std::string &filename, const brayns::LoaderProgress &callback,
    const brayns::PropertyMap &properties) const
{
    PLUGIN_INFO << "Loading circuit from " << filename << std::endl;
    callback.updateProgress("Loading circuit ...", 0);
    brayns::PropertyMap props = _defaults;
    props.merge(_fixedDefaults);
    props.merge(properties);
    return {importCircuit(filename, props, callback)};
}

std::string PairSynapsesLoader::getName() const
{
    return LOADER_NAME;
}

brayns::PropertyMap PairSynapsesLoader::getCLIProperties()
{
    brayns::PropertyMap pm("Pair-Synapses Loader");
    pm.add(PROP_PRESYNAPTIC_NEURON_GID);
    pm.add(PROP_POSTSYNAPTIC_NEURON_GID);
    pm.add(PROP_RADIUS_MULTIPLIER);
    pm.add(PROP_SECTION_TYPE_SOMA);
    pm.add(PROP_SECTION_TYPE_AXON);
    pm.add(PROP_SECTION_TYPE_DENDRITE);
    pm.add(PROP_SECTION_TYPE_APICAL_DENDRITE);
    pm.add(PROP_USE_SDF_GEOMETRY);
    pm.add(PROP_MORPHOLOGY_COLOR_SCHEME);
    pm.add(PROP_MORPHOLOGY_QUALITY);
    pm.add(PROP_SYNAPSE_RADIUS);
    return pm;
}
