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

#include "ColorMethodUtils.h"

#include <brayns/engine/components/ColorList.h>
#include <brayns/engine/components/ColorSolid.h>
#include <brayns/engine/components/GeometryViews.h>

#include <ospray/ospray_cpp/ext/rkcommon.h>

namespace brayns
{
std::vector<brayns::Vector4f> &ColorListComponentUtils::createAndInit(brayns::Components &components, size_t length)
{
    auto &colorList = components.getOrAdd<brayns::ColorList>();
    auto &colors = colorList.elements;

    if (!colors.empty())
    {
        return colors;
    }

    auto solidColor = components.find<brayns::ColorSolid>();
    if (solidColor)
    {
        colors.resize(length, solidColor->color);
        return colors;
    }

    colors.resize(length, brayns::Vector4f(1.f));
    return colors;
}

void ColorListComponentUtils::apply(brayns::Components &components)
{
    auto colorList = components.find<brayns::ColorList>();
    if (!colorList)
    {
        throw std::invalid_argument("The model does not have a color list to apply");
    }

    apply(components, colorList->elements);
}

void ColorListComponentUtils::apply(brayns::Components &components, const std::vector<brayns::Vector4f> &colors)
{
    auto &views = components.get<brayns::GeometryViews>();
    auto &view = views.elements.front();
    view.setColorPerPrimitive(ospray::cpp::SharedData(colors));
    views.modified = true;
}
}
