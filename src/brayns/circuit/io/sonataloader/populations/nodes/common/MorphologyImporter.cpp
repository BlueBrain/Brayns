/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <brayns/circuit/api/circuit/MorphologyCircuitBuilder.h>
#include <brayns/circuit/io/sonataloader/colordata/ColorDataFactory.h>
#include <brayns/circuit/io/sonataloader/data/Cells.h>
#include <brayns/circuit/io/sonataloader/populations/nodes/common/NeuronReportFactory.h>

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
        return sonataloader::Cells::getPositions(context.population, context.selection);
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
void MorphologyImporter::import(NodeLoadContext &context, const std::vector<brayns::Quaternion> &rotations)
{
    NeuronMetadataFactory::create(context);

    auto ids = DataExtractor::extractIds(context);
    auto positions = DataExtractor::extractPositions(context);
    auto morphologyPaths = DataExtractor::extractMorphologyPaths(context);
    auto neuronParams = DataExtractor::extractNeuronParams(context);
    auto buildContext = MorphologyCircuitBuilder::Context{
        std::move(ids),
        std::move(morphologyPaths),
        std::move(positions),
        std::move(rotations),
        neuronParams,
        ColorDataFactory::create(context)};

    auto compartments = MorphologyCircuitBuilder::build(context.model, std::move(buildContext), context.progress);
    NeuronReportFactory::create(context, compartments);
}
}
