/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include <io/sonataloader/data/Cells.h>
#include <io/sonataloader/data/Names.h>
#include <io/sonataloader/populations/nodes/common/MorphologyImporter.h>
#include <io/sonataloader/populations/nodes/common/NeuronReportFactory.h>
#include <io/sonataloader/populations/nodes/common/SomaImporter.h>

#include <brayns/utils/Log.h>

namespace
{
class RotationExtractor
{
public:
    static std::vector<brayns::Quaternion> extract(
        const bbp::sonata::NodePopulation &population,
        const bbp::sonata::Selection &selection)
    {
        try
        {
            return sonataloader::Cells::getRotations(population, selection);
        }
        catch (...)
        {
        }

        brayns::Log::warn("[CE] Cannot retrieve '{}' rotations, using identity", population.name());
        return std::vector<brayns::Quaternion>(selection.flatSize(), brayns::Quaternion(1, 0, 0, 0));
    }
};
}

namespace sonataloader
{
std::string_view BiophysicalPopulationLoader::getPopulationType() const noexcept
{
    return NodeNames::biophysical;
}

void BiophysicalPopulationLoader::load(NodeLoadContext &context) const
{
    auto &loadParams = context.params;
    auto &morphParams = loadParams.neuron_morphology_parameters;
    auto soma = morphParams.load_soma;
    auto axon = morphParams.load_axon;
    auto dend = morphParams.load_dendrites;

    if (soma && !axon && !dend)
    {
        SomaImporter::import(context);
        return;
    }

    auto &population = context.population;
    auto &selection = context.selection;
    auto rotations = RotationExtractor::extract(population, selection);
    MorphologyImporter::import(context, rotations);
}
} // namespace sonataloader
