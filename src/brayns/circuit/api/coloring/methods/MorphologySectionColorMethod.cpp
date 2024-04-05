/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
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

#include "MorphologySectionColorMethod.h"

#include <brayns/core/engine/colormethods/ColorMethodUtils.h>

#include <brayns/circuit/components/NeuronSectionGeometryMap.h>

#include <cassert>

MorphologySectionColorMethod::MorphologySectionColorMethod(size_t primitiveCount):
    _primitiveCount(primitiveCount)
{
    assert(_primitiveCount > 0);
}

std::string MorphologySectionColorMethod::getName() const
{
    return "morphology section geometry";
}

std::vector<std::string> MorphologySectionColorMethod::getValues(brayns::Components &components) const
{
    auto &sectionGeometryMapping = components.get<NeuronSectionGeometryMap>().mapping;

    auto result = std::vector<std::string>();
    result.reserve(sectionGeometryMapping.size());

    for (auto &section : sectionGeometryMapping)
    {
        result.push_back(std::to_string(section.sectionId));
    }

    std::sort(result.begin(), result.end());
    return result;
}

void MorphologySectionColorMethod::apply(brayns::Components &components, const brayns::ColorMethodInput &input) const
{
    auto &sectionGeometryMapping = components.get<NeuronSectionGeometryMap>().mapping;
    auto &colors = brayns::ColorListComponentUtils::createAndInit(components, _primitiveCount);

    for (auto &section : sectionGeometryMapping)
    {
        auto it = input.find(std::to_string(section.sectionId));
        if (it == input.end())
        {
            continue;
        }

        auto begin = colors.begin();
        auto end = colors.begin();

        std::advance(begin, section.begin);
        std::advance(end, section.end);

        auto &color = it->second;

        std::fill(begin, end, color);
    }

    brayns::ColorListComponentUtils::apply(components, colors);
}
