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

#include <brayns/common/types.h>
#include <ospray/SDK/common/Material.h>
#include <ospray/SDK/texture/Texture2D.h>

namespace brayns
{
namespace obj
{
typedef ospray::vec3f Color;

struct ExtendedOBJMaterial : public ospray::Material
{
    /*! opacity: 0 (transparent), 1 (opaque) */
    ospray::Texture2D* map_d;
    ospray::affine2f xform_d;
    float d;

    /*! refraction index */
    ospray::Texture2D* map_Refraction;
    ospray::affine2f xform_Refraction;
    float refraction;

    /*! reflection index */
    ospray::Texture2D* map_Reflection;
    ospray::affine2f xform_Reflection;
    float reflection;

    /*! radiance: 0 (none), 1 (full) */
    ospray::Texture2D* map_a;
    ospray::affine2f xform_a;
    float a;

    /*! diffuse  reflectance: 0 (none), 1 (full) */
    ospray::Texture2D* map_Kd;
    ospray::affine2f xform_Kd;
    Color Kd;

    /*! specular reflectance: 0 (none), 1 (full) */
    ospray::Texture2D* map_Ks;
    ospray::affine2f xform_Ks;
    Color Ks;

    /*! specular exponent: 0 (diffuse), infinity (specular) */
    ospray::Texture2D* map_Ns;
    ospray::affine2f xform_Ns;
    float Ns;

    /*! Glossiness: 0 (none), 1 (full) */
    float glossiness;

    /*! bump map */
    ospray::Texture2D* map_Bump;
    ospray::affine2f xform_Bump;
    ospray::linear2f rot_Bump;

    /*! Casts simulation data */
    bool castSimulationData;

    /*! Shading mode (none, diffuse, electron, etc) */
    MaterialShadingMode shadingMode;

    std::string toString() const final
    {
        return "brayns::extendedobjrenderer::ExtendedOBJMaterial";
    }

    void commit() final;
};

} // namespace obj
} // namespace brayns
