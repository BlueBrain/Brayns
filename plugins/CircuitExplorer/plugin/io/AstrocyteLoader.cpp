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

#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>
#include <common/commonTypes.h>
#include <common/log.h>

#include <fstream>

const std::string LOADER_NAME = "Astrocyte batch loader";
const std::string SUPPORTED_EXTENTION_ASTROCYTES = "astrocytes";

AstrocyteLoader::AstrocyteLoader(
    brayns::Scene &scene,
    const brayns::ApplicationParameters &applicationParameters,
    brayns::PropertyMap &&loaderParams, CircuitExplorerPlugin* plugin)
    : AbstractCircuitLoader(scene, applicationParameters,
                            std::move(loaderParams), plugin)
{
    PLUGIN_INFO << "Registering " << LOADER_NAME << std::endl;
    _fixedDefaults.setProperty(
        {PROP_DB_CONNECTION_STRING.name, std::string("")});
    _fixedDefaults.setProperty({PROP_DENSITY.name, 1.0});
    _fixedDefaults.setProperty({PROP_RANDOM_SEED.name, 0.0});
    _fixedDefaults.setProperty({PROP_REPORT.name, std::string("")});
    _fixedDefaults.setProperty({PROP_TARGETS.name, std::string("")});
    _fixedDefaults.setProperty({PROP_GIDS.name, std::string("")});
    _fixedDefaults.setProperty(
        {PROP_REPORT_TYPE.name, enumToString(ReportType::undefined)});
    _fixedDefaults.setProperty({PROP_RADIUS_MULTIPLIER.name, 1.0});
    _fixedDefaults.setProperty({PROP_RADIUS_CORRECTION.name, 0.0});
    _fixedDefaults.setProperty({PROP_CIRCUIT_COLOR_SCHEME.name,
                                enumToString(CircuitColorScheme::by_id)});
    _fixedDefaults.setProperty(
        {PROP_DAMPEN_BRANCH_THICKNESS_CHANGERATE.name, true});
    _fixedDefaults.setProperty({PROP_USE_REALISTIC_SOMA.name, false});
    _fixedDefaults.setProperty({PROP_METABALLS_SAMPLES_FROM_SOMA.name, 0});
    _fixedDefaults.setProperty({PROP_METABALLS_GRID_SIZE.name, 0});
    _fixedDefaults.setProperty({PROP_METABALLS_THRESHOLD.name, 0.0});
    _fixedDefaults.setProperty(
        {PROP_USER_DATA_TYPE.name, enumToString(UserDataType::undefined)});
    _fixedDefaults.setProperty({PROP_MORPHOLOGY_MAX_DISTANCE_TO_SOMA.name,
                                std::numeric_limits<double>::max()});
    _fixedDefaults.setProperty({PROP_MESH_FOLDER.name, std::string("")});
    _fixedDefaults.setProperty(
        {PROP_MESH_FILENAME_PATTERN.name, std::string("")});
    _fixedDefaults.setProperty({PROP_MESH_TRANSFORMATION.name, false});
    _fixedDefaults.setProperty({PROP_CELL_CLIPPING.name, false});
    _fixedDefaults.setProperty({PROP_AREAS_OF_INTEREST.name, 0});
    _fixedDefaults.setProperty({PROP_SYNAPSE_RADIUS.name, 1.0});
    _fixedDefaults.setProperty({PROP_LOAD_AFFERENT_SYNAPSES.name, true});
    _fixedDefaults.setProperty({PROP_LOAD_EFFERENT_SYNAPSES.name, true});
    _fixedDefaults.setProperty(
        {PROP_PRESYNAPTIC_NEURON_GID.name, std::string("")});
    _fixedDefaults.setProperty(
        {PROP_POSTSYNAPTIC_NEURON_GID.name, std::string("")});
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
    PLUGIN_INFO << "Loading morphologies from " << filename << std::endl;
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
    pm.setProperty(PROP_SECTION_TYPE_SOMA);
    pm.setProperty(PROP_SECTION_TYPE_AXON);
    pm.setProperty(PROP_SECTION_TYPE_DENDRITE);
    pm.setProperty(PROP_SECTION_TYPE_APICAL_DENDRITE);
    pm.setProperty(PROP_USE_SDF_GEOMETRY);
    pm.setProperty(PROP_MORPHOLOGY_COLOR_SCHEME);
    pm.setProperty(PROP_MORPHOLOGY_QUALITY);
    return pm;
}

void AstrocyteLoader::_importMorphologiesFromURIs(
    const brayns::PropertyMap &properties, const std::vector<std::string> &uris,
    const brayns::LoaderProgress &callback, brayns::Model &model) const
{
    brayns::PropertyMap morphologyProps(properties);
    MorphologyLoader loader(_scene, std::move(morphologyProps));

    const auto colorScheme = stringToEnum<MorphologyColorScheme>(
        properties.getProperty<std::string>(PROP_MORPHOLOGY_COLOR_SCHEME.name));

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
    materialProps.setProperty({MATERIAL_PROPERTY_CAST_USER_DATA, false});
    materialProps.setProperty({MATERIAL_PROPERTY_SHADING_MODE,
                               static_cast<int>(MaterialShadingMode::diffuse)});
    materialProps.setProperty(
        {MATERIAL_PROPERTY_CLIPPING_MODE,
         static_cast<int>(MaterialClippingMode::no_clipping)});
    MorphologyLoader::createMissingMaterials(model, materialProps);
}
