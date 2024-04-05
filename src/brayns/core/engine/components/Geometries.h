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

#pragma once

#include <brayns/core/engine/geometry/Geometry.h>
#include <brayns/core/utils/ModifiedFlag.h>

namespace brayns
{
struct Geometries
{
    Geometries() = default;

    template<typename T>
    explicit Geometries(T primitive)
    {
        elements.emplace_back(std::move(primitive));
    }

    template<typename T>
    explicit Geometries(std::vector<T> primitives)
    {
        elements.emplace_back(std::move(primitives));
    }

    template<typename T>
    explicit Geometries(std::vector<std::vector<T>> primitivesList)
    {
        elements.reserve(primitivesList.size());
        for (auto &primitives : primitivesList)
        {
            elements.emplace_back(std::move(primitives));
        }
    }

    std::vector<Geometry> elements;
    ModifiedFlag modified;
};
}
