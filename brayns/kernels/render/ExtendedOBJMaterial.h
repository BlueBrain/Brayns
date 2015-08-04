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

#pragma once

#include <ospray/common/Material.h>
#include <ospray/texture/Texture2D.h>

using namespace ospray;

namespace brayns
{
namespace obj
{
typedef vec3f Color;

struct ExtendedOBJMaterial : public Material
{
    /*! opacity: 0 (transparent), 1 (opaque) */
    Texture2D *map_d;
    float d;

    /*! refaction index */
    Texture2D *map_r;
    float r;

    /*! radiance: 0 (none), 1 (full) */
    Texture2D *map_a;
    float a;

    /*! diffuse  reflectance: 0 (none), 1 (full) */
    Texture2D *map_Kd;
    Color Kd;

    /*! specular reflectance: 0 (none), 1 (full) */
    Texture2D *map_Ks;
    Color Ks;

    /*! specular exponent: 0 (diffuse), infinity (specular) */
    Texture2D *map_Ns;
    float Ns;

    /*! bump map */
    Texture2D *map_Bump;

    virtual std::string toString() const {
        return "brayns::extendedobjrenderer::ExtendedOBJMaterial"; }

    virtual void commit();
};

} // ::brayns::obj
} // ::brayns
