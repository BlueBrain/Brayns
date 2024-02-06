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

#include "Phong.h"

#include <ospray/ospray_cpp/ext/rkcommon.h>

namespace
{
struct DefaultParameters
{
    static inline const std::string diffuseColor = "kd";
    static inline const std::string shininess = "ns";
    static inline const std::string opacity = "d";
};
}

namespace brayns
{
void MaterialTraits<Phong>::updateData(ospray::cpp::Material &handle, Phong &data)
{
    handle.setParam(DefaultParameters::diffuseColor, Vector3f(1.f));
    handle.setParam(DefaultParameters::shininess, 2.f);
    handle.setParam(DefaultParameters::opacity, data.opacity);
}
}
