
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

#include "Plastic.h"

#include <ospray/ospray_cpp/ext/rkcommon.h>

namespace
{
struct PlasticParameters
{
    static inline const std::string color = "baseColor";
    static inline const std::string roughness = "roughness";
    static inline const std::string coat = "coat";
    static inline const std::string coatThickness = "coatThickness";
    static inline const std::string opacity = "opacity";
};
}

namespace brayns
{
void MaterialTraits<Plastic>::updateData(ospray::cpp::Material &handle, Plastic &data)
{
    handle.setParam(PlasticParameters::color, Vector3f(1.f));
    handle.setParam(PlasticParameters::roughness, 0.01f);
    handle.setParam(PlasticParameters::coat, 1.f);
    handle.setParam(PlasticParameters::coatThickness, 3.f);
    handle.setParam(PlasticParameters::opacity, data.opacity);
}
}
