/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/engine/components/Bounds.h>

#include <ospray/ospray_cpp/Geometry.h>

#include <string>
#include <vector>

namespace brayns
{
template<typename T>
constexpr bool geometrySpecialized = false;

template<typename T>
class GeometryTraits
{
public:
    static inline const std::string handleName;
    static inline const std::string name;

    static Bounds computeBounds(const TransformMatrix &matrix, const T &data)
    {
        static_assert(geometrySpecialized<T>, "GeometryTraits not specialized");
        (void)matrix;
        (void)data;
        return {};
    }

    static void update(ospray::cpp::Geometry &handle, std::vector<T> &data)
    {
        static_assert(geometrySpecialized<T>, "GeometryTraits not specialized");
        (void)handle;
        (void)data;
    }
};
}
