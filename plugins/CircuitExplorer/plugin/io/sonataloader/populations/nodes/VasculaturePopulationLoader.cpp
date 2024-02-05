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

#include "VasculaturePopulationLoader.h"

#include <plugin/io/morphology/vasculature/VasculatureInstance.h>
#include <plugin/io/sonataloader/data/SonataVasculature.h>

namespace sonataloader
{
std::vector<MorphologyInstance::Ptr> VasculaturePopulationLoader::load(
    const SonataConfig::Data& networkData,
    const SonataNodePopulationParameters& lc,
    const bbp::sonata::Selection& selection) const
{
    const auto population =
        networkData.config.getNodePopulation(lc.node_population);

    const auto startPoints =
        SonataVasculature::getSegmentStartPoints(population, selection);
    const auto endPoints =
        SonataVasculature::getSegmentEndPoints(population, selection);
    const auto sectionTypes =
        SonataVasculature::getSegmentSectionTypes(population, selection);

    std::vector<float> startRadii, endRadii;
    const auto radOverride = lc.vasculature_geometry_parameters.radius_override;
    const auto radMultiplier =
        lc.vasculature_geometry_parameters.radius_multiplier;
    if (radOverride > 0.f)
    {
        startRadii.resize(startPoints.size(), radOverride);
        endRadii.resize(endPoints.size(), radOverride);
    }
    else
    {
        startRadii =
            SonataVasculature::getSegmentStartRadii(population, selection);
        endRadii = SonataVasculature::getSegmentEndRadii(population, selection);
        if (radMultiplier != 1.f)
        {
            std::cout << "Modding" << std::endl;
            std::transform(startRadii.begin(), startRadii.end(),
                           startRadii.begin(),
                           [mult = radMultiplier](const float r) {
                               return r * mult;
                           });
            std::transform(endRadii.begin(), endRadii.end(), endRadii.begin(),
                           [mult = radMultiplier](const float r) {
                               return r * mult;
                           });
        }
    }

    std::vector<MorphologyInstance::Ptr> result(startPoints.size());

#pragma omp parallel for
    for (size_t i = 0; i < startPoints.size(); ++i)
        result[i] =
            std::make_unique<VasculatureInstance>(startPoints[i], startRadii[i],
                                                  endPoints[i], endRadii[i],
                                                  sectionTypes[i]);

    return result;
}
} // namespace sonataloader
