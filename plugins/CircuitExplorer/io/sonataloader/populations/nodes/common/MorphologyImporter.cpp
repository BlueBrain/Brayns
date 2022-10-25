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

#include "NeuronMetadataFactory.h"

#include <api/circuit/MorphologyCircuitBuilder.h>
#include <io/sonataloader/data/Cells.h>
#include <io/sonataloader/populations/nodes/common/NeuronReportFactory.h>

namespace sonataloader
{
void MorphologyImporter::import(
    NodeLoadContext &ctxt,
    const std::vector<brayns::Quaternion> &rotations,
    std::unique_ptr<IColorData> colorData)
{
    NeuronMetadataFactory::create(ctxt);

    auto &population = ctxt.population;
    auto populationName = population.name();
    auto &selection = ctxt.selection;
    auto flatSelection = selection.flatten();

    auto positions = Cells::getPositions(population, selection);
    auto morphologies = Cells::getMorphologies(population, selection);

    auto &params = ctxt.params;
    auto &neuronParams = params.neuron_morphology_parameters;

    auto &config = ctxt.config;
    auto pathBuilder = config.getMorphologyPath(populationName);

    auto morphologyPaths = std::vector<std::string>();
    morphologyPaths.reserve(morphologies.size());
    for (auto &morphology : morphologies)
    {
        morphologyPaths.push_back(pathBuilder.buildPath(morphology));
    }

    auto &model = ctxt.model;
    auto &cb = ctxt.progress;
    MorphologyCircuitBuilder::Context context(flatSelection, morphologyPaths, positions, rotations, neuronParams);

    auto compartments = MorphologyCircuitBuilder::load(context, model, cb, std::move(colorData));
    NeuronReportFactory::create(ctxt, compartments);
}
}
