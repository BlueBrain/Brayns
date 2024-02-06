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

#include "SolidColorMethod.h"

#include <brayns/engine/components/ColorList.h>
#include <brayns/engine/components/ColorMap.h>
#include <brayns/engine/components/ColorSolid.h>
#include <brayns/engine/components/GeometryViews.h>

#include <spdlog/fmt/fmt.h>

namespace
{
struct SolidMethodNames
{
    static inline const std::string name = "solid";
    static inline const std::string value = "color";
};

class ColorFiller
{
public:
    static void fill(brayns::Components &components, const brayns::Vector4f &color)
    {
        _fillColorList(components, color);
        _fillColorMap(components, color);
    }

private:
    static void _fillColorList(brayns::Components &components, const brayns::Vector4f &color)
    {
        auto colorList = components.find<brayns::ColorList>();
        if (!colorList)
        {
            return;
        }

        auto &colors = colorList->elements;
        std::fill(colors.begin(), colors.end(), color);
    }

    static void _fillColorMap(brayns::Components &components, const brayns::Vector4f &color)
    {
        auto colorMap = components.find<brayns::ColorMap>();
        if (!colorMap)
        {
            return;
        }

        auto &colors = colorMap->colors;
        std::fill(colors.begin(), colors.end(), color);
    }
};

}

namespace brayns
{
std::string SolidColorMethod::getName() const
{
    return SolidMethodNames::name;
}

std::vector<std::string> SolidColorMethod::getValues(Components &components) const
{
    (void)components;
    return {SolidMethodNames::value};
}

void SolidColorMethod::apply(Components &components, const ColorMethodInput &input) const
{
    auto &solidColor = components.getOrAdd<ColorSolid>();
    auto it = input.find(SolidMethodNames::value);
    if (it == input.end())
    {
        throw std::invalid_argument(fmt::format("Missing solid method input key '{}'", SolidMethodNames::value));
    }
    solidColor.color = it->second;

    auto &views = components.get<GeometryViews>();
    for (auto &view : views.elements)
    {
        view.setColor(solidColor.color);
    }

    ColorFiller::fill(components, solidColor.color);

    views.modified = true;
}
}
