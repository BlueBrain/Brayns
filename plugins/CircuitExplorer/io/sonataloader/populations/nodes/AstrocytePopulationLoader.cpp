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

#include <io/sonataloader/colordata/node/AstrocyteColorData.h>
#include <io/sonataloader/populations/nodes/common/ColorDataFactory.h>
#include <io/sonataloader/populations/nodes/common/MorphologyImporter.h>
#include <io/sonataloader/populations/nodes/common/SomaImporter.h>

namespace sonataloader
{
std::string AstrocytePopulationLoader::getPopulationType() const noexcept
{
    return "astrocyte";
}

void AstrocytePopulationLoader::load(NodeLoadContext &context) const
{
    auto colorData = NodeColorDataFactory::create<AstrocyteColorData>(context);

    const auto &loadParams = context.params;
    const auto &morphParams = loadParams.neuron_morphology_parameters;
    const auto soma = morphParams.load_soma;
    const auto axon = morphParams.load_axon;
    const auto dend = morphParams.load_dendrites;

    if (soma && !axon && !dend)
    {
        SomaImporter::import(context, std::move(colorData));
        return;
    }

    const auto &selection = context.selection;
    std::vector<brayns::Quaternion> dummyRotations(selection.flatSize());
    MorphologyImporter::import(context, dummyRotations, std::move(colorData));
}
} // namespace sonataloader
