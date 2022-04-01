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

#include "VasculatureColorData.h"

#include <plugin/io/morphology/vasculature/VasculatureSection.h>
#include <plugin/io/sonataloader/data/SonataVasculature.h>

namespace sonataloader
{
namespace
{
constexpr char methodBySection[] = "vasculature_section";
} // namespace

VasculatureColorData::VasculatureColorData(bbp::sonata::CircuitConfig config, std::string populationName)
 : _config(std::move(config))
 , _population(std::move(populationName))
{
}

std::vector<std::string> VasculatureColorData::getMethods() const noexcept
{
    return {methodBySection};
}

std::vector<std::string> VasculatureColorData::getMethodVariables(const std::string &method) const
{
    if(method == methodBySection)
    {
        return brayns::enumNames<VasculatureSection>();
    }
    else
    {
        throw std::invalid_argument("Method " + method + " not available to color vasculature");
    }
}

std::vector<std::string> VasculatureColorData::getMethodValuesForIDs(
        const std::string &method, const std::vector<uint64_t> &ids) const
{
    if(method != methodBySection)
    {
        throw std::invalid_argument("Method " + method + "not available to color vasculature");
    }

    const auto &population = _config.getNodePopulation(_population);
    const auto selection = bbp::sonata::Selection::fromValues(ids);
    const auto sections = SonataVasculature::getSegmentSectionTypes(population, selection);
    std::vector<std::string> result;
    result.reserve(sections.size());
    for(const auto section : sections)
    {
        const auto sectionName = brayns::enumToString<VasculatureSection>(section);
        result.push_back(sectionName);
    }

    return result;
}
} // namespace sonataloader
