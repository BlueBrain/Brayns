/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
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

#include "DefaultMaterial.ih"

export void* uniform DefaultMaterial_create(void* uniform cppE)
{
    uniform DefaultMaterial* uniform mat = uniform new uniform DefaultMaterial;
    mat->super.cppEquivalent = cppE;
    return mat;
}

export void DefaultMaterial_set(
    void* uniform _mat, const void* uniform map_d,
    const uniform affine2f& xform_d, const uniform float& d,
    const void* uniform map_refraction,
    const uniform affine2f& xform_refraction, const uniform float& refraction,
    const void* uniform map_reflection,
    const uniform affine2f& xform_reflection, const uniform float& reflection,
    const void* uniform map_a, const uniform affine2f& xform_a,
    const uniform float& a, const uniform float& glossiness,
    const void* uniform map_Kd, const uniform affine2f& xform_Kd,
    const uniform vec3f& Kd, const void* uniform map_Ks,
    const uniform affine2f& xform_Ks, const uniform vec3f& Ks,
    const void* uniform map_Ns, const uniform affine2f& xform_Ns,
    const uniform float& Ns, const void* uniform map_Bump,
    const uniform affine2f& xform_Bump, const uniform linear2f& rot_Bump)
{
    uniform DefaultMaterial* uniform self =
        (uniform DefaultMaterial * uniform)_mat;

    self->map_d = make_TextureParam((Texture2D*)map_d, xform_d);
    self->d = d;
    self->map_refraction =
        make_TextureParam((Texture2D*)map_refraction, xform_refraction);
    self->refraction = refraction;
    self->map_reflection =
        make_TextureParam((Texture2D*)map_reflection, xform_reflection);
    self->reflection = reflection;
    self->map_a = make_TextureParam((Texture2D*)map_a, xform_a);
    self->a = a;
    self->glossiness = glossiness;
    self->map_Kd = make_TextureParam((Texture2D*)map_Kd, xform_Kd);
    self->Kd = Kd;
    self->map_Ks = make_TextureParam((Texture2D*)map_Ks, xform_Ks);
    self->Ks = Ks;
    self->map_Ns = make_TextureParam((Texture2D*)map_Ns, xform_Ns);
    self->Ns = Ns;
    self->map_Bump = make_TextureParam((Texture2D*)map_Bump, xform_Bump);
    self->rot_Bump = rot_Bump;
    self->volume = 0;
}
