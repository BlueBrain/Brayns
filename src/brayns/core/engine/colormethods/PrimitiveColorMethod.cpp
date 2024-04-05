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

#include "PrimitiveColorMethod.h"

#include "ColorMethodUtils.h"

#include <brayns/core/utils/parsing/Parser.h>

#include <cassert>

namespace
{
class IndexListExtractor
{
public:
    static std::vector<size_t> fromRange(const std::string &range, size_t maxIndex)
    {
        auto indexList = brayns::Parser::parseRange<size_t>(range);
        if (indexList.back() >= maxIndex)
        {
            throw std::range_error("Range out of bounds: " + range);
        }
        return indexList;
    }
};
}

namespace brayns
{
PrimitiveColorMethod::PrimitiveColorMethod(std::string name, size_t primitiveCount):
    _name(std::move(name)),
    _primitiveCount(primitiveCount)
{
    assert(_primitiveCount > 0);
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
    auto &colors = ColorListComponentUtils::createAndInit(components, _primitiveCount);

    for (auto &[range, color] : input)
    {
        auto indexList = IndexListExtractor::fromRange(range, _primitiveCount);
        for (auto index : indexList)
        {
            colors[index] = color;
        }
    }

    ColorListComponentUtils::apply(components, colors);
}
}
