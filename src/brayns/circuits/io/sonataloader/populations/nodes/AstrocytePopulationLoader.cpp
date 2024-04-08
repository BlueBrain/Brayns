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

#include "AstrocytePopulationLoader.h"

#include <brayns/circuits/io/sonataloader/data/Names.h>
#include <brayns/circuits/io/sonataloader/populations/nodes/common/MorphologyImporter.h>
#include <brayns/circuits/io/sonataloader/populations/nodes/common/SomaImporter.h>

namespace sonataloader
{
std::string_view AstrocytePopulationLoader::getPopulationType() const noexcept
{
    return NodeNames::astrocyte;
}

void AstrocytePopulationLoader::load(NodeLoadContext &context) const
{
    auto &loadParams = context.params;
    auto &morphParams = loadParams.neuron_morphology_parameters;
    auto soma = morphParams.load_soma;
    auto axon = morphParams.load_axon;
    auto dend = morphParams.load_dendrites;

    if (!soma && !axon && !dend)
    {
        SomaImporter::import(context);
        return;
    }

    auto &selection = context.selection;
    auto dummyRotations = std::vector<brayns::Quaternion>(selection.flatSize(), brayns::Quaternion(1, 0, 0, 0));
    MorphologyImporter::import(context, dummyRotations);
}
} // namespace sonataloader
