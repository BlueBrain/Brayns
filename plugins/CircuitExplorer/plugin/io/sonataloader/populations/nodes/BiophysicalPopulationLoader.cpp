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

#include "BiophysicalPopulationLoader.h"

#include <plugin/io/sonataloader/data/SonataCells.h>

namespace sonataloader
{
std::vector<MorphologyInstance::Ptr> BiophysicalPopulationLoader::load(
    const SonataConfig::Data& networkData,
    const SonataNodePopulationParameters& lc,
    const bbp::sonata::Selection& nodeSelection) const
{
    const auto population =
        networkData.config.getNodePopulation(lc.node_population);

    const auto morphologies =
        SonataCells::getMorphologies(population, nodeSelection);
    const auto positions = SonataCells::getPositions(population, nodeSelection);
    const auto rotations = SonataCells::getRotations(population, nodeSelection);

    return loadNodes(networkData, lc, nodeSelection, morphologies, positions,
                     rotations);
}
} // namespace sonataloader
