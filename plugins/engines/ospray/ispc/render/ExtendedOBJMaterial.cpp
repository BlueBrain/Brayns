/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * Based on OSPRay implementation
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

#include "ExtendedOBJMaterial.h"
#include "ExtendedOBJMaterial_ispc.h"
#include <ospray/SDK/common/Data.h>

#define OSP_REGISTER_EXMATERIAL(InternalClassName, external_name)          \
    extern "C" ospray::Material *ospray_create_material__##external_name() \
    {                                                                      \
        return new InternalClassName;                                      \
    }

namespace brayns
{
namespace obj
{
void ExtendedOBJMaterial::commit()
{
    if (ispcEquivalent == nullptr)
        ispcEquivalent = ispc::ExtendedOBJMaterial_create(this);

    map_d = (ospray::Texture2D *)getParamObject("map_d", nullptr);
    map_refraction = (ospray::Texture2D *)getParamObject(
        "map_Refraction", getParamObject("map_refraction", nullptr));
    map_reflection = (ospray::Texture2D *)getParamObject(
        "map_Reflection", getParamObject("map_reflection", nullptr));
    map_a = (ospray::Texture2D *)getParamObject("map_a", nullptr);
    map_Kd =
        (ospray::Texture2D *)getParamObject("map_Kd",
                                            getParamObject("map_kd", nullptr));
    map_Ks =
        (ospray::Texture2D *)getParamObject("map_Ks",
                                            getParamObject("map_ks", nullptr));
    map_Ns =
        (ospray::Texture2D *)getParamObject("map_Ns",
                                            getParamObject("map_ns", nullptr));
    map_Bump = (ospray::Texture2D *)getParamObject("map_Bump",
                                                   getParamObject("map_bump",
                                                                  nullptr));
    map_Normal = (ospray::Texture2D *)
        getParamObject("map_Normal", getParamObject("map_normal", nullptr));

    d = getParam1f("d", 1.f);
    refraction = getParam1f("refraction", 0.f);
    reflection = getParam1f("reflection", 0.f);
    a = getParam1f("a", 0.f);
    Kd = getParam3f("kd", getParam3f("Kd", ospray::vec3f(.8f)));
    Ks = getParam3f("ks", getParam3f("Ks", ospray::vec3f(0.f)));
    Ns = getParam1f("ns", getParam1f("Ns", 10.f));

    ispc::ExtendedOBJMaterial_set(
        getIE(), map_d ? map_d->getIE() : nullptr, d,
        map_refraction ? map_refraction->getIE() : nullptr, refraction,
        map_reflection ? map_reflection->getIE() : nullptr, reflection,
        map_a ? map_a->getIE() : nullptr, a, map_Kd ? map_Kd->getIE() : nullptr,
        (ispc::vec3f &)Kd, map_Ks ? map_Ks->getIE() : nullptr,
        (ispc::vec3f &)Ks, map_Ns ? map_Ns->getIE() : nullptr, Ns,
        map_Bump != nullptr ? map_Bump->getIE() : nullptr,
        map_Normal != nullptr ? map_Normal->getIE() : nullptr);
}

OSP_REGISTER_EXMATERIAL(ExtendedOBJMaterial, ExtendedOBJMaterial);
} // ::brayns::obj
} // ::brayns
