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

#include "SolidColorMethod.h"

#include <brayns/engine/components/ColorList.h>
#include <brayns/engine/components/GeometryViews.h>

namespace
{
struct SolidMethodNames
{
    inline static const std::string name = "solid";
    inline static const std::string value = "color";
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
    auto &color = input.at(SolidMethodNames::value);

    auto &views = components.get<GeometryViews>();
    for (auto &view : views.elements)
    {
        view.setColor(color);
    }

    views.modified = true;
}
}
