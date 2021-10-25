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

#include "AstrocyteLoader.h"

#include <brayns/common/Log.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>

#include <common/commonTypes.h>

#include <fstream>

const std::string LOADER_NAME = "Astrocyte batch loader";
const std::string SUPPORTED_EXTENTION_ASTROCYTES = "astrocytes";

AstrocyteLoader::AstrocyteLoader(
    brayns::Scene &scene,
    const brayns::ApplicationParameters &applicationParameters,
    brayns::PropertyMap &&loaderParams, CircuitExplorerPlugin *plugin)
    : AbstractCircuitLoader(scene, applicationParameters,
                            std::move(loaderParams), plugin)
{
    brayns::Log::info("[CE] Registering {}.", LOADER_NAME);
    _fixedDefaults.add({PROP_DB_CONNECTION_STRING.getName(), std::string("")});
    _fixedDefaults.add({PROP_DENSITY.getName(), 1.0});
    _fixedDefaults.add({PROP_RANDOM_SEED.getName(), 0.0});
    _fixedDefaults.add({PROP_REPORT.getName(), std::string("")});
    _fixedDefaults.add({PROP_TARGETS.getName(), std::string("")});
    _fixedDefaults.add({PROP_GIDS.getName(), std::string("")});
    _fixedDefaults.add(
        {PROP_REPORT_TYPE.getName(), enumToString(ReportType::undefined)});
    _fixedDefaults.add({PROP_RADIUS_MULTIPLIER.getName(), 1.0});
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
    _fixedDefaults.add({PROP_SYNAPSE_RADIUS.getName(), 1.0});
    _fixedDefaults.add({PROP_LOAD_AFFERENT_SYNAPSES.getName(), true});
    _fixedDefaults.add({PROP_LOAD_EFFERENT_SYNAPSES.getName(), true});
    _fixedDefaults.add(
        {PROP_PRESYNAPTIC_NEURON_GID.getName(), std::string("")});
    _fixedDefaults.add(
        {PROP_POSTSYNAPTIC_NEURON_GID.getName(), std::string("")});
}

std::vector<std::string> AstrocyteLoader::getSupportedExtensions() const
{
    return {SUPPORTED_EXTENTION_ASTROCYTES};
}

bool AstrocyteLoader::isSupported(const std::string & /*filename*/,
                                  const std::string &extension) const
{
    const std::set<std::string> types = {SUPPORTED_EXTENTION_ASTROCYTES};
    return types.find(extension) != types.end();
}

std::vector<brayns::ModelDescriptorPtr> AstrocyteLoader::importFromFile(
    const std::string &filename, const brayns::LoaderProgress &callback,
    const brayns::PropertyMap &properties) const
{
    brayns::Log::info("[CE] Loading morphologies from {}.", filename);
    callback.updateProgress("Loading morphologies ...", 0);
    brayns::PropertyMap props = _defaults;
    props.merge(_fixedDefaults);
    props.merge(properties);

    std::vector<std::string> uris;
    std::ifstream file(filename);
    if (file.is_open())
    {
        std::string line;
        while (getline(file, line))
            uris.push_back(line);
        file.close();
    }

    auto model = _scene.createModel();
    brayns::ModelDescriptorPtr modelDescriptor;
    _importMorphologiesFromURIs(props, uris, callback, *model);
    modelDescriptor =
        std::make_shared<brayns::ModelDescriptor>(std::move(model), filename);
    return {modelDescriptor};
}

std::string AstrocyteLoader::getName() const
{
    return LOADER_NAME;
}

brayns::PropertyMap AstrocyteLoader::getCLIProperties()
{
    brayns::PropertyMap pm("AstrocyteLoader");
    pm.add(PROP_SECTION_TYPE_SOMA);
    pm.add(PROP_SECTION_TYPE_AXON);
    pm.add(PROP_SECTION_TYPE_DENDRITE);
    pm.add(PROP_SECTION_TYPE_APICAL_DENDRITE);
    pm.add(PROP_USE_SDF_GEOMETRY);
    pm.add(PROP_MORPHOLOGY_COLOR_SCHEME);
    pm.add(PROP_MORPHOLOGY_QUALITY);
    return pm;
}

void AstrocyteLoader::_importMorphologiesFromURIs(
    const brayns::PropertyMap &properties, const std::vector<std::string> &uris,
    const brayns::LoaderProgress &callback, brayns::Model &model) const
{
    brayns::PropertyMap morphologyProps(properties);
    MorphologyLoader loader(_scene, std::move(morphologyProps));

    const auto colorScheme = stringToEnum<MorphologyColorScheme>(
        properties[PROP_MORPHOLOGY_COLOR_SCHEME.getName()].to<std::string>());

    for (uint64_t i = 0; i < uris.size(); ++i)
    {
        const auto uri = brion::URI(uris[i]);
        const auto materialId =
            (colorScheme == MorphologyColorScheme::none ? i
                                                        : brayns::NO_MATERIAL);

        loader.setDefaultMaterialId(materialId);

        MorphologyInfo morphologyInfo;
        morphologyInfo = loader.importMorphology(morphologyProps, uri, model, i,
                                                 brayns::Matrix4f(), nullptr,
                                                 nullptr, nullptr);
        callback.updateProgress("Loading morphologies...",
                                (float)i / (float)uris.size());
    }
    brayns::PropertyMap materialProps;
    materialProps.add({MATERIAL_PROPERTY_CAST_USER_DATA, false});
    materialProps.add({MATERIAL_PROPERTY_SHADING_MODE,
                       static_cast<int>(MaterialShadingMode::diffuse)});
    materialProps.add({MATERIAL_PROPERTY_CLIPPING_MODE,
                       static_cast<int>(MaterialClippingMode::no_clipping)});
    MorphologyLoader::createMissingMaterials(model, materialProps);
}
