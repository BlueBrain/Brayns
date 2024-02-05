/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include "PointNeuronPopulationLoader.h"

#include <plugin/io/morphology/neuron/instances/SampleNeuronInstance.h>
#include <plugin/io/sonataloader/data/SonataCells.h>

namespace sonataloader
{
std::vector<MorphologyInstance::Ptr> PointNeuronPopulationLoader::load(
    const SonataConfig::Data& networkData,
    const SonataNodePopulationParameters& lc,
    const bbp::sonata::Selection& nodeSelection) const
{
    const auto population =
        networkData.config.getNodePopulation(lc.node_population);
    const auto nodesSize = nodeSelection.flatSize();
    const auto positions = SonataCells::getPositions(population, nodeSelection);
    const auto radMult = lc.neuron_morphology_parameters.radius_multiplier;
    const auto radOverride = lc.neuron_morphology_parameters.radius_override;
    const float radius =
        radOverride > 0.f ? radOverride : (radMult > 0.f ? radMult : 1.f);
    std::vector<MorphologyInstance::Ptr> result(nodesSize);

    auto sharedData = std::make_shared<SampleSharedData>();
    sharedData->sectionMap.insert({-1, {0}});
    sharedData->sectionTypeMap.insert({NeuronSection::SOMA, {0}});

#pragma omp parallel for
    for (size_t i = 0; i < nodesSize; ++i)
    {
        result[i] = std::make_unique<SampleNeuronInstance>(
            std::vector<brayns::Sphere>{brayns::Sphere(positions[i], radius)},
            sharedData);
    }

    return result;
}
} // namespace sonataloader
