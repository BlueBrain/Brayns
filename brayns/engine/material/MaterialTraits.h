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

#include <ospray/ospray_cpp/Material.h>

#include <cassert>
#include <string>

namespace brayns
{
template<typename T>
constexpr bool materialSpecialized = false;

template<typename T>
class MaterialTraits
{
public:
    static inline const std::string handleName;
    static inline const std::string name;

    static void updateData(ospray::cpp::Material &handle, T &data)
    {
        static_assert(materialSpecialized<T>, "MaterialTraits not specialized");
        (void)handle;
        (void)data;
    }
};
}
