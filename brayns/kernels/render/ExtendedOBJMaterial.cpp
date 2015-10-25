/* Copyright (c) 2011-2015, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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
#include <ospray/common/Data.h>

namespace brayns
{
namespace obj
{
void ExtendedOBJMaterial::commit()
{
    if (ispcEquivalent == NULL)
        ispcEquivalent = ispc::ExtendedOBJMaterial_create(this);

    map_d  = (Texture2D*)getParamObject("map_d", NULL);
    map_refraction = (Texture2D*)getParamObject(
                "map_Refraction", getParamObject("map_refraction", NULL));
    map_reflection = (Texture2D*)getParamObject(
                "map_Reflection", getParamObject("map_reflection", NULL));
    map_a  = (Texture2D*)getParamObject("map_a", NULL);
    map_Kd = (Texture2D*)getParamObject("map_Kd",
                                        getParamObject("map_kd", NULL));
    map_Ks = (Texture2D*)getParamObject("map_Ks",
                                        getParamObject("map_ks", NULL));
    map_Ns = (Texture2D*)getParamObject("map_Ns",
                                        getParamObject("map_ns", NULL));
    map_Bump = (Texture2D*)getParamObject("map_Bump",
                                          getParamObject("map_bump", NULL));
    map_Normal = (Texture2D*)getParamObject("map_Normal",
                                          getParamObject("map_normal", NULL));

    d  = getParam1f("d", 1.f);
    refraction = getParam1f("refraction", 0.f);
    reflection = getParam1f("reflection", 0.f);
    a  = getParam1f("a", 0.f);
    Kd = getParam3f("kd", getParam3f("Kd", vec3f(.8f)));
    Ks = getParam3f("ks", getParam3f("Ks", vec3f(0.f)));
    Ns = getParam1f("ns", getParam1f("Ns", 10.f));

    ispc::ExtendedOBJMaterial_set(
                getIE(),
                map_d ? map_d->getIE() : NULL,
                d,
                map_refraction ? map_refraction->getIE() : NULL,
                refraction,
                map_reflection ? map_reflection->getIE() : NULL,
                reflection,
                map_a ? map_a->getIE() : NULL,
                a,
                map_Kd ? map_Kd->getIE() : NULL,
                (ispc::vec3f&)Kd,
                map_Ks ? map_Ks->getIE() : NULL,
                (ispc::vec3f&)Ks,
                map_Ns ? map_Ns->getIE() : NULL,
                Ns,
                map_Bump != NULL ? map_Bump->getIE() : NULL,
                map_Normal != NULL ? map_Normal->getIE() : NULL);
}

OSP_REGISTER_MATERIAL(ExtendedOBJMaterial,ExtendedOBJMaterial);
} // ::brayns::obj
} // ::brayns
