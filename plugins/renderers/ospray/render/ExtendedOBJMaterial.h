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

#pragma once

#include <ospray/common/Material.h>
#include <ospray/texture/Texture2D.h>

namespace brayns
{
namespace obj
{

typedef ospray::vec3f Color;

struct ExtendedOBJMaterial : public ospray::Material
{
    /*! opacity: 0 (transparent), 1 (opaque) */
    ospray::Texture2D *map_d;
    float d;

    /*! refraction index */
    ospray::Texture2D *map_refraction;
    float refraction;

    /*! reflection index */
    ospray::Texture2D *map_reflection;
    float reflection;

    /*! radiance: 0 (none), 1 (full) */
    ospray::Texture2D *map_a;
    float a;

    /*! diffuse  reflectance: 0 (none), 1 (full) */
    ospray::Texture2D *map_Kd;
    Color Kd;

    /*! specular reflectance: 0 (none), 1 (full) */
    ospray::Texture2D *map_Ks;
    Color Ks;

    /*! specular exponent: 0 (diffuse), infinity (specular) */
    ospray::Texture2D *map_Ns;
    float Ns;

    /*! normal map */
    ospray::Texture2D *map_Normal;

    /*! bump map */
    ospray::Texture2D *map_Bump;

    std::string toString() const final {
        return "brayns::extendedobjrenderer::ExtendedOBJMaterial"; }

    void commit() final;
};

} // ::brayns::obj
} // ::brayns
