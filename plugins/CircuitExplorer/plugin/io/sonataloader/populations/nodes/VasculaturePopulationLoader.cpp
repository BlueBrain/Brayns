/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
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

#include <plugin/api/Log.h>
#include <plugin/io/morphology/vasculature/VasculatureInstance.h>
#include <plugin/io/sonataloader/data/SonataVasculature.h>

namespace sonataloader
{
namespace
{
VasculatureSection __computeVasculatureSection(const VasculatureGeometrySettings& vgs)
{
    VasculatureSection section = VasculatureSection::NONE;
    if(vgs.load_arterial_capillary)
        section |= VasculatureSection::ARTERIAL_CAPILLARY;
    if(vgs.load_arteriole)
        section |= VasculatureSection::ARTERIOLE;
    if(vgs.load_artery)
        section |= VasculatureSection::ARTERY;
    if(vgs.load_transitional)
        section |= VasculatureSection::TRANSITIONAL;
    if(vgs.load_vein)
        section |= VasculatureSection::VEIN;
    if(vgs.load_venous_capillary)
        section |= VasculatureSection::VENOUS_CAPILLARY;
    if(vgs.load_venule)
        section |= VasculatureSection::VENULE;

    return section;
}
}

std::vector<MorphologyInstance::Ptr> VasculaturePopulationLoader::load(
    const SonataConfig::Data& networkData,
    const SonataNodePopulationParameters& lc,
    const bbp::sonata::Selection& selection) const
{
    const auto population = networkData.config.getNodePopulation(lc.node_population);

    const auto startPoints =
        SonataVasculature::getSegmentStartPoints(population, selection);
    const auto endPoints =
        SonataVasculature::getSegmentEndPoints(population, selection);
    const auto sectionTypes =
        SonataVasculature::getSegmentSectionTypes(population, selection);

    std::vector<float> startRadii, endRadii;
    const auto radOverride = lc.vasculature_geometry_parameters.radius_override;
    const auto radMultiplier = lc.vasculature_geometry_parameters.radius_multiplier;
    if (radOverride > 0.f)
    {
        startRadii.resize(startPoints.size(), radOverride);
        endRadii.resize(endPoints.size(), radOverride);
    }
    else
    {
        startRadii =
            SonataVasculature::getSegmentStartRadii(population, selection);
        endRadii =
            SonataVasculature::getSegmentEndRadii(population, selection);
        if (radMultiplier != 1.f)
        {
            std::transform(startRadii.begin(), startRadii.end(),
                           startRadii.begin(),
                           [mult = radMultiplier](
                               const float r) { return r * mult; });
            std::transform(endRadii.begin(), endRadii.end(), endRadii.begin(),
                           [mult = radMultiplier](
                               const float r) { return r * mult; });
        }
    }

    std::vector<MorphologyInstance::Ptr> result(startPoints.size());
    const auto requestedSections = __computeVasculatureSection(lc.vasculature_geometry_parameters);

    PLUGIN_WARN << "Vasculature section check disabled. Test data has wrong "
                   "'type' dataset"
                << std::endl;

    #pragma omp parallel for
    for (size_t i = 0; i < startPoints.size(); ++i)
    {
        if (sectionTypes[i] != VasculatureSection::NONE &&
            !static_cast<uint8_t>(sectionTypes[i] & requestedSections))
            continue;

        result[i] =
            std::make_unique<VasculatureInstance>(startPoints[i], startRadii[i],
                                                  endPoints[i], endRadii[i],
                                                  sectionTypes[i]);
    }

    return result;
}
} // namespace sonataloader
