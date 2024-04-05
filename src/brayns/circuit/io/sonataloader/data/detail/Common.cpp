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

#include "Common.h"

#include <spdlog/fmt/fmt.h>

namespace sonataloader::detail
{
std::vector<std::string> EnumExtractor::extract(
    const bbp::sonata::Population &population,
    const bbp::sonata::Selection &selection,
    const std::string &attribute)
{
    auto enumValues = population.enumerationValues(attribute);
    auto enumIndices = population.getEnumeration<size_t>(attribute, selection);

    auto result = std::vector<std::string>();
    result.reserve(enumIndices.size());

    for (auto index : enumIndices)
    {
        result.push_back(enumValues[index]);
    }

    return result;
}

void AttributeValidator::validate(const bbp::sonata::Population &population, const std::vector<std::string> &attribs)
{
    auto &attributes = population.attributeNames();
    for (auto &attrib : attribs)
    {
        if (attributes.find(attrib) != attributes.end())
        {
            continue;
        }

        throw std::runtime_error(fmt::format("Population '{}' is missing '{}'", population.name(), attrib));
    }
}
}
