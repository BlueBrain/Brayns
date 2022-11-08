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

#include "SomaImporter.h"

#include "NeuronMetadataFactory.h"

#include <api/circuit/SomaCircuitBuilder.h>
#include <io/sonataloader/colordata/ColorDataFactory.h>
#include <io/sonataloader/data/Cells.h>
#include <io/sonataloader/populations/nodes/common/NeuronReportFactory.h>

namespace sonataloader
{
void SomaImporter::import(NodeLoadContext &nodeContext)
{
    NeuronMetadataFactory::create(nodeContext);

    auto &population = nodeContext.population;
    auto &selection = nodeContext.selection;

    auto ids = selection.flatten();

    auto positions = Cells::getPositions(population, selection);

    auto &params = nodeContext.params;
    auto &neuronParams = params.neuron_morphology_parameters;

    auto radius = neuronParams.radius_multiplier;

    auto &model = nodeContext.model;

    SomaCircuitBuilder::Context context(ids, positions, radius);

    auto colorData = ColorDataFactory::create(nodeContext);

    auto compartments = SomaCircuitBuilder::load(context, model, std::move(colorData));
    NeuronReportFactory::create(nodeContext, compartments);
}
}
