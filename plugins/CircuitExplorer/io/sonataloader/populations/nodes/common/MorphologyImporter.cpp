/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman <nadir.romanguerrero@epfl.ch>
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

#include "MorphologyImporter.h"

#include <api/circuit/MorphologyCircuitBuilder.h>
#include <io/sonataloader/data/SonataCells.h>
#include <io/sonataloader/populations/nodes/common/NeuronReportFactory.h>

namespace sonataloader
{
void MorphologyImporter::import(
    NodeLoadContext &ctxt,
    const std::vector<brayns::Quaternion> &rotations,
    std::unique_ptr<IColorData> colorData)
{
    namespace sl = sonataloader;

    const auto &population = ctxt.population;
    const auto populationName = population.name();
    const auto &selection = ctxt.selection;
    const auto flatSelection = selection.flatten();

    const auto positions = sl::SonataCells::getPositions(population, selection);
    const auto morphologies = sl::SonataCells::getMorphologies(population, selection);

    const auto &params = ctxt.params;
    const auto &neuronParams = params.neuron_morphology_parameters;

    const auto &network = ctxt.config;
    const auto &config = network.circuitConfig();
    const auto populationProperties = config.getNodePopulationProperties(populationName);
    const auto morphologyPathBuilder = sl::SonataConfig::resolveMorphologyPath(populationProperties);

    auto morphologyPaths = std::vector<std::string>(morphologies.size());
    for (size_t i = 0; i < morphologies.size(); ++i)
    {
        const auto &morphology = morphologies[i];
        morphologyPaths[i] = morphologyPathBuilder.buildPath(morphology);
    }

    auto &model = ctxt.model;
    auto &cb = ctxt.progress;

    MorphologyCircuitBuilder::Context context(flatSelection, morphologyPaths, positions, rotations, neuronParams);

    const auto compartments = MorphologyCircuitBuilder::load(context, model, cb, std::move(colorData));

    NeuronReportFactory::create(ctxt, compartments);
}
}
