/* Copyright (c) 2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel Nachbaur <daniel.nachbaur@epfl.ch>
 *
 * This file is part of https://github.com/BlueBrain/ospray-modules
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

#include "Material.h"

#include "../Context.h"

#include <ospray/SDK/common/Data.h>

namespace bbp
{
namespace optix
{
Material::~Material()
{
    if (optixMaterial)
        optixMaterial->destroy();
}

void Material::commit()
{
    auto map_Kd =
        (ospray::Texture2D*)getParamObject("map_Kd",
                                           getParamObject("map_kd", nullptr));

    const float d = getParam1f("d", 1.f);
    const float refraction = getParam1f("refraction", 0.f);
    const float reflection = getParam1f("reflection", 0.f);
    const float g = getParam1f("g", getParam1f("glossiness", 0.f));
    const ospray::vec3f Kd =
        getParam3f("kd", getParam3f("Kd", ospray::vec3f(.8f)));
    const ospray::vec3f Ks =
        getParam3f("ks", getParam3f("Ks", ospray::vec3f(0.f)));
    const float Ns = getParam1f("ns", getParam1f("Ns", 10.f));
    const bool skybox = getParam1i("skybox", 0) == 1;
    const unsigned int shadingMode = getParam1i("shading_mode", 0);

    if (!optixMaterial)
        optixMaterial = Context::get().createMaterial(map_Kd);

    if (map_Kd)
    {
        if (skybox)
        {
            Context::get().getOptixContext()["envmap"]->setTextureSampler(
                Context::get().getTextureSampler(map_Kd));
        }
        else
        {
            optixMaterial["diffuse_map"]->setTextureSampler(
                Context::get().getTextureSampler(map_Kd));
        }
    }

    optixMaterial["Kd"]->setFloat(Kd.x, Kd.y, Kd.z);
    optixMaterial["Ks"]->setFloat(Ks.x, Ks.y, Ks.z);
    optixMaterial["phong_exp"]->setFloat(Ns);
    optixMaterial["Kr"]->setFloat(reflection, reflection, reflection);
    optixMaterial["Ko"]->setFloat(d, d, d);
    optixMaterial["refraction_index"]->setFloat(refraction);
    optixMaterial["glossiness"]->setFloat(g);
    optixMaterial["shading_mode"]->setInt(shadingMode);
}
} // namespace optix
} // namespace bbp
