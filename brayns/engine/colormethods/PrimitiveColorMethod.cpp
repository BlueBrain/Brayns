/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include "PrimitiveColorMethod.h"

#include <brayns/engine/components/ColorList.h>
#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/components/GeometryViews.h>

#include <brayns/utils/parsing/Parser.h>

namespace
{
class PrimitiveCount
{
public:
    static size_t fromGeometry(const brayns::Components &components)
    {
        auto &geometries = components.get<brayns::Geometries>();
        auto &geometry = geometries.elements.front();
        return geometry.numPrimitives();
    }
};

class ColorListExtractor
{
public:
    static std::vector<brayns::Vector4f> &fromComponents(brayns::Components &components, size_t length)
    {
        auto &colorList = components.getOrAdd<brayns::ColorList>();
        auto &colors = colorList.elements;
        colors.resize(length, brayns::Vector4f(1.f));
        return colors;
    }
};

class IndexListExtractor
{
public:
    static std::vector<size_t> fromRange(const std::string &range, size_t maxIndex)
    {
        auto indexList = brayns::Parser::extractRanges<size_t>(range);
        if (indexList.back() >= maxIndex)
        {
            throw std::range_error("Range out of bounds: " + range);
        }
        return indexList;
    }
};

class Painter
{
public:
    static void apply(const std::vector<brayns::Vector4f> &colors, brayns::Components &components)
    {
        auto &views = components.get<brayns::GeometryViews>();
        auto &view = views.elements.front();
        view.setColorPerPrimitive(ospray::cpp::SharedData(colors));
        views.modified = true;
    }
};
}

namespace brayns
{
PrimitiveColorMethod::PrimitiveColorMethod(std::string name, size_t primitiveCount)
    : _name(name)
    , _primitiveCount(primitiveCount)
{
}

std::string PrimitiveColorMethod::getName() const
{
    return _name;
}

std::vector<std::string> PrimitiveColorMethod::getValues(Components &components) const
{
    (void)components;
    return {"0-" + std::to_string(_primitiveCount - 1)};
}

void PrimitiveColorMethod::apply(Components &components, const ColorMethodInput &input) const
{
    auto &colors = ColorListExtractor::fromComponents(components, _primitiveCount);

    for (auto &[range, color] : input)
    {
        auto indexList = IndexListExtractor::fromRange(range, _primitiveCount);
        for (auto index : indexList)
        {
            colors[index] = color;
        }
    }

    Painter::apply(colors, components);
}
}
