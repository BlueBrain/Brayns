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

#include <api/vasculature/VasculatureColorMethod.h>
#include <io/sonataloader/data/SonataVasculature.h>

namespace sonataloader
{
VasculatureColorData::VasculatureColorData(bbp::sonata::NodePopulation nodes)
    : _nodes(std::move(nodes))
{
}

std::vector<std::string> VasculatureColorData::getMethods() const noexcept
{
    return brayns::EnumInfo::getNames<VasculatureColorMethod>();
}

std::vector<std::string> VasculatureColorData::getMethodVariables(const std::string &method) const
{
    const auto enumMethod = brayns::EnumInfo::getValue<VasculatureColorMethod>(method);

    if (enumMethod == VasculatureColorMethod::BySection)
    {
        return brayns::EnumInfo::getNames<VasculatureSection>();
    }

    throw std::invalid_argument("Method " + method + " not available to color vasculature");
}

std::vector<std::string> VasculatureColorData::getMethodValuesForIDs(
    const std::string &method,
    const std::vector<uint64_t> &ids) const
{
    const auto enumMethod = brayns::EnumInfo::getValue<VasculatureColorMethod>(method);

    if (enumMethod != VasculatureColorMethod::BySection)
    {
        throw std::invalid_argument("Method " + method + "not available to color vasculature");
    }

    const auto selection = bbp::sonata::Selection::fromValues(ids);
    const auto sections = SonataVasculature::getSegmentSectionTypes(_nodes, selection);
    std::vector<std::string> result;
    result.reserve(sections.size());
    for (const auto section : sections)
    {
        auto sectionName = brayns::EnumInfo::getName(section);
        result.emplace_back(std::move(sectionName));
    }

    return result;
}
} // namespace sonataloader
