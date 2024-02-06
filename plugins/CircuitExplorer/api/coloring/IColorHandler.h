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

#pragma once

#include <brayns/engine/components/ColorList.h>
#include <brayns/engine/components/ColorMap.h>
#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/components/GeometryViews.h>

/**
 * @brief Interface to implement coloring methods on different geometry arrangements.
 */
class IColorHandler
{
public:
    virtual ~IColorHandler() = default;

    /**
     * @brief Applies the colors of a ColorList component to the underlying geometry.
     * @param colors The color source.
     * @param views The geometry to color.
     */
    virtual void colorByElement(const brayns::ColorList &colors, brayns::GeometryViews &views) const = 0;

    /**
     * @brief Applies the colors of an indexed ColorMap component to the underlying geometry.
     * @param colors The colormap source.
     * @params geometries The primitives of the geometry.
     * @param views The geometry to color.
     */
    virtual void colorByColormap(
        const brayns::ColorMap &colors,
        const brayns::Geometries &geometries,
        brayns::GeometryViews &views) const = 0;
};
