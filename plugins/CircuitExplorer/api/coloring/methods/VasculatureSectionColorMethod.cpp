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

#include "VasculatureSectionColorMethod.h"

#include <brayns/engine/common/ExtractColor.h>
#include <brayns/engine/components/GeometryViews.h>

#include <components/ColorHandler.h>
#include <components/VasculatureSectionList.h>

#include <algorithm>
#include <unordered_set>

std::string VasculatureSectionColorMethod::getName() const
{
    return "vasculature section type";
}

std::vector<std::string> VasculatureSectionColorMethod::getValues(brayns::Components &components) const
{
    auto &sections = components.get<VasculatureSectionList>().sections;
    auto uniqueSections = std::unordered_set<VasculatureSection>(sections.begin(), sections.end());

    auto sectionNames = std::vector<std::string>();
    sectionNames.reserve(uniqueSections.size());

    for (auto section : uniqueSections)
    {
        sectionNames.push_back(brayns::EnumInfo::getName(section));
    }

    std::sort(sectionNames.begin(), sectionNames.end());
    return sectionNames;
}

void VasculatureSectionColorMethod::apply(brayns::Components &components, const brayns::ColorMethodInput &input) const
{
    auto &sections = components.get<VasculatureSectionList>().sections;
    auto &colors = brayns::ExtractColor::extractList(components, sections.size());

    for (size_t i = 0; i < sections.size(); ++i)
    {
        auto section = sections[i];
        auto sectionName = brayns::EnumInfo::getName(section);
        auto it = input.find(sectionName);
        if (it == input.end())
        {
            continue;
        }

        colors.elements[i] = it->second;
    }

    auto &views = components.get<brayns::GeometryViews>();
    auto &handler = *components.get<ColorHandler>().handler;
    handler.colorByElement(colors, views);
}
