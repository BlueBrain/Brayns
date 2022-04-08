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

#include "AstrocytePopulationLoader.h"

#include <api/circuit/MorphologyCircuitBuilder.h>
#include <io/sonataloader/colordata/node/AstrocyteColorData.h>
#include <io/sonataloader/data/SonataCells.h>
#include <io/sonataloader/data/SonataConfig.h>
#include <io/sonataloader/populations/nodes/common/ColorDataFactory.h>
#include <io/sonataloader/populations/nodes/common/NeuronReportFactory.h>
#include <io/sonataloader/populations/nodes/common/SomaImporter.h>

namespace
{
namespace sl = sonataloader;

struct AstrocyteMorphologyImporter
{
    static std::vector<CellCompartments> import(sl::NodeLoadContext &ctxt, std::unique_ptr<IColorData> colorData)
    {
        const auto &population = ctxt.population;
        const auto populationName = population.name();
        const auto &selection = ctxt.selection;
        const auto flatSelection = selection.flatten();

        const auto positions = sl::SonataCells::getPositions(population, selection);
        const auto morphologies = sl::SonataCells::getMorphologies(population, selection);
        const auto rotations = std::vector<brayns::Quaternion>(positions.size(), brayns::Quaternion());

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

        return MorphologyCircuitBuilder::load(context, model, cb, std::move(colorData));
    }
};
}

namespace sonataloader
{
std::string AstrocytePopulationLoader::getPopulationType() const noexcept
{
    return "astrocyte";
}

void AstrocytePopulationLoader::load(NodeLoadContext &ctxt) const
{
    auto colorData = NodeColorDataFactory::create<AstrocyteColorData>(ctxt);

    const auto &loadParams = ctxt.params;
    const auto &morphParams = loadParams.neuron_morphology_parameters;
    const auto soma = morphParams.load_soma;
    const auto axon = morphParams.load_axon;
    const auto dend = morphParams.load_dendrites;

    std::vector<CellCompartments> compartments;

    if (soma && !axon && !dend)
    {
        compartments = SomaImporter::import(ctxt, std::move(colorData));
    }
    else
    {
        compartments = AstrocyteMorphologyImporter::import(ctxt, std::move(colorData));
    }

    NeuronReportFactory::create(ctxt, compartments);
}
} // namespace sonataloader
