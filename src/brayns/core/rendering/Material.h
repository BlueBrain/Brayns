/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include "Managed.h"

namespace brayns::experimental
{
class Material : public Managed<OSPMaterial>
{
public:
    using Managed::Managed;
};

class ObjMaterial : public Material
{
public:
    static inline const std::string name = "obj";

    using Material::Material;

    void setDiffuseColor(const Color3 &color);
    void setSpecularColor(const Color3 &color);
    void setShininess(float exponent);
    void setOpacity(float opacity);
    void setTransparencyFilter(const Color3 &color);
};
}

namespace ospray
{
OSPTYPEFOR_SPECIALIZATION(brayns::experimental::Material, OSP_MATERIAL)
OSPTYPEFOR_SPECIALIZATION(brayns::experimental::ObjMaterial, OSP_MATERIAL)
}