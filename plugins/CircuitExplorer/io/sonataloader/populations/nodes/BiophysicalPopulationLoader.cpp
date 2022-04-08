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

#include "BiophysicalPopulationLoader.h"

#include <io/circuit/MorphologyCircuitLoader.h>
#include <io/circuit/SomaCircuitLoader.h>

#include <io/sonataloader/colordata/node/BiophysicalColorData.h>
#include <io/sonataloader/data/SonataCells.h>
#include <io/sonataloader/data/SonataConfig.h>
#include <io/sonataloader/populations/nodes/common/ColorDataFactory.h>
#include <io/sonataloader/populations/nodes/common/SomaImporter.h>

namespace
{
struct BiophysicalMorphologyImporter
{
    static std::vector<CellCompartments> import(
        const sonataloader::NodeLoadContext &ctxt,
        ProgressUpdater &cb,
        brayns::Model &model,
        std::unique_ptr<IColorData> colorData)
    {
        const auto &population = ctxt.population;
        const auto populationName = population.name();
        const auto &selection = ctxt.selection;
        const auto flatSelection = selection.flatten();

        const auto positions = sonataloader::SonataCells::getPositions(population, selection);
        const auto morphologies = sonataloader::SonataCells::getMorphologies(population, selection);
        const auto rotations = sonataloader::SonataCells::getRotations(population, selection);

        const auto &params = ctxt.params;
        const auto &neuronParams = params.neuron_morphology_parameters;

        const auto &network = ctxt.config;
        const auto &config = network.circuitConfig();
        const auto populationProperties = config.getNodePopulationProperties(populationName);
        const auto morphologyPathBuilder = sonataloader::SonataConfig::resolveMorphologyPath(populationProperties);

        auto morphologyPaths = std::vector<std::string>(morphologies.size());
        for (size_t i = 0; i < morphologies.size(); ++i)
        {
            const auto &morphology = morphologies[i];
            morphologyPaths[i] = morphologyPathBuilder.buildPath(morphology);
        }

        MorphologyCircuitLoader::Context context(flatSelection, morphologyPaths, positions, rotations, neuronParams);

        return MorphologyCircuitLoader::load(context, model, cb, std::move(colorData));
    }
};
}

namespace sonataloader
{
std::string BiophysicalPopulationLoader::getPopulationType() const noexcept
{
    return "biophysical";
}

std::vector<CellCompartments>
    BiophysicalPopulationLoader::load(const NodeLoadContext &ctxt, ProgressUpdater &cb, brayns::Model &model) const
{
    auto colorData = NodeColorDataFactory::create<BiophysicalColorData>(ctxt);

    const auto &loadParams = ctxt.params;
    const auto &morphParams = loadParams.neuron_morphology_parameters;
    const auto soma = morphParams.load_soma;
    const auto axon = morphParams.load_axon;
    const auto dend = morphParams.load_dendrites;

    if (soma && !axon && !dend)
    {
        return SomaImporter::import(ctxt, model, std::move(colorData));
    }

    return BiophysicalMorphologyImporter::import(ctxt, cb, model, std::move(colorData));
}
} // namespace sonataloader
