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

#include "EndfeetColorHandler.h"

void EndfeetColorHandler::colorByElement(const brayns::ColorList &colors, brayns::GeometryViews &views) const
{
    auto &colorList = colors.elements;
    auto &viewList = views.elements;

    for (size_t i = 0; i < viewList.size(); ++i)
    {
        auto &view = viewList[i];
        view.setColor(colorList[i]);
    }

    views.modified = true;
}

void EndfeetColorHandler::colorByColormap(
    const brayns::ColorMap &colorMap,
    const brayns::Geometries &geometries,
    brayns::GeometryViews &views) const
{
    (void)colorMap;
    (void)geometries;
    (void)views;
    assert(false);
}
