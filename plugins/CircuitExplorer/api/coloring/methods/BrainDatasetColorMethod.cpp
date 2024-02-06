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

#include "BrainDatasetColorMethod.h"

#include <brayns/engine/common/ExtractColor.h>
#include <brayns/engine/components/GeometryViews.h>

#include <components/BrainColorData.h>
#include <components/CircuitIds.h>
#include <components/ColorHandler.h>

#include <unordered_set>

BrainDatasetColorMethod::BrainDatasetColorMethod(BrainColorMethod method):
    _method(method)
{
}

std::string BrainDatasetColorMethod::getName() const
{
    return brayns::EnumInfo::getName(_method);
}

std::vector<std::string> BrainDatasetColorMethod::getValues(brayns::Components &components) const
{
    auto &ids = components.get<CircuitIds>().elements;
    auto &data = *components.get<BrainColorData>().data;
    auto allValues = data.getValues(_method, ids);
    auto uniqueValues = std::unordered_set<std::string>(allValues.begin(), allValues.end());
    auto values = std::vector<std::string>(uniqueValues.begin(), uniqueValues.end());
    std::sort(values.begin(), values.end());
    return values;
}

void BrainDatasetColorMethod::apply(brayns::Components &components, const brayns::ColorMethodInput &input) const
{
    auto &colorData = *components.get<BrainColorData>().data;
    auto &ids = components.get<CircuitIds>().elements;

    auto &colors = brayns::ExtractColor::extractList(components, ids.size());

    auto allValues = colorData.getValues(_method, ids);

    for (size_t i = 0; i < ids.size(); ++i)
    {
        auto it = input.find(allValues[i]);
        if (it == input.end())
        {
            continue;
        }

        colors.elements[i] = it->second;
    }

    auto &views = components.get<brayns::GeometryViews>();
    auto &painter = *components.get<ColorHandler>().handler;
    painter.colorByElement(colors, views);
}
