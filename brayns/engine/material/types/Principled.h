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

#include <brayns/engine/material/MaterialTraits.h>
#include <brayns/utils/MathTypes.h>

namespace brayns
{
struct Principled
{
    Vector3f edgeColor = Vector3f(1.f);
    float metallic = 0.f;
    float diffuse = 1.f;
    float specular = 1.f;
    float ior = 1.f;
    float transmission = 0.f;
    Vector3f transmissionColor = Vector3f(1.f);
    float transmissionDepth = 1.f;
    float roughness = 0.f;
    float anisotropy = 0.f;
    float anisotropyRotation = 0.f;
    bool thin = false;
    float thickness = 1.f;
    float backLight = 0.f;
    float coat = 0.f;
    float coatIor = 1.5f;
    Vector3f coatColor = Vector3f(1.f);
    float coatThickness = 1.f;
    float coatRoughness = 0.f;
    float sheen = 0.f;
    Vector3f sheenColor = Vector3f(1.f);
    float sheenTint = 0.f;
    float sheenRoughness = 0.2f;
};

template<>
class MaterialTraits<Principled>
{
public:
    static inline const std::string handleName = "principled";
    static inline const std::string name = "principled";

    static void updateData(ospray::cpp::Material &handle, Principled &data);
};
}
