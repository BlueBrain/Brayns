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
#include <io/sonataloader/colordata/ColorDataFactory.h>
#include <io/sonataloader/data/Cells.h>
#include <io/sonataloader/populations/nodes/common/NeuronReportFactory.h>

namespace
{
class DataExtractor
{
public:
    static std::vector<uint64_t> extractIds(const sonataloader::NodeLoadContext &context)
    {
        auto &selection = context.selection;
        return selection.flatten();
    }

    static std::vector<brayns::Vector3f> extractPositions(const sonataloader::NodeLoadContext &context)
    {
        auto positions = sonataloader::Cells::getPositions(context.population, context.selection);
    }

    static std::vector<std::string> extractMorphologyPaths(const sonataloader::NodeLoadContext &context)
    {
        auto &population = context.population;
        auto &selection = context.selection;
        auto morphologies = sonataloader::Cells::getMorphologies(population, selection);

        auto &config = context.config;
        auto pathBuilder = config.getMorphologyPath(population.name());

        auto morphologyPaths = std::vector<std::string>();
        morphologyPaths.reserve(morphologies.size());

        for (auto &morphology : morphologies)
        {
            morphologyPaths.push_back(pathBuilder.buildPath(morphology));
        }

        return morphologyPaths;
    }

    static NeuronMorphologyLoaderParameters extractNeuronParams(const sonataloader::NodeLoadContext &context)
    {
        return context.params.neuron_morphology_parameters;
    }
};
}

namespace sonataloader
{
void MorphologyImporter::import(NodeLoadContext &nodeContext, const std::vector<brayns::Quaternion> &rotations)
{
    NeuronMetadataFactory::create(nodeContext);

    auto ids = DataExtractor::extractIds(nodeContext);
    auto positions = DataExtractor::extractPositions(nodeContext);
    auto morphologyPaths = DataExtractor::extractMorphologyPaths(nodeContext);
    auto neuronParams = DataExtractor::extractNeuronParams(nodeContext);
    auto context = MorphologyCircuitBuilder::Context(ids, morphologyPaths, positions, rotations, neuronParams);

    auto &model = nodeContext.model;

    auto &progressCallback = nodeContext.progress;

    auto colorData = ColorDataFactory::create(nodeContext);

    auto compartments = MorphologyCircuitBuilder::load(context, model, progressCallback, std::move(colorData));
    NeuronReportFactory::create(nodeContext, compartments);
}
}
