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

#include "ExtendedOBJMaterial.ih"

export void *uniform ExtendedOBJMaterial_create(void *uniform cppE)
{
    uniform ExtendedOBJMaterial *uniform mat =
            uniform new uniform ExtendedOBJMaterial;
    mat->base.cppEquivalent = cppE;
    return mat;
}

export void ExtendedOBJMaterial_set(
        void *uniform _mat,
        const void *uniform map_d,
        const uniform float &d,
        const void *uniform map_refraction,
        const uniform float &refraction,
        const void *uniform map_reflection,
        const uniform float &reflection,
        const void *uniform map_a,
        const uniform float &a,
        const void *uniform map_Kd,
        const uniform vec3f &Kd,
        const void *uniform map_Ks,
        const uniform vec3f &Ks,
        const void *uniform map_Ns,
        const uniform float &Ns,
        const void *uniform map_Bump,
        const void *uniform map_Normal)
{
    uniform ExtendedOBJMaterial *uniform self =
            (uniform ExtendedOBJMaterial *uniform)_mat;

    self->map_d = (uniform Texture2D *uniform)map_d;
    self->d = d;
    self->map_refraction = (uniform Texture2D *uniform)map_refraction;
    self->refraction = refraction;
    self->map_reflection = (uniform Texture2D *uniform)map_reflection;
    self->reflection = reflection;
    self->map_a = (uniform Texture2D *uniform)map_a;
    self->a = a;
    self->map_Kd = (uniform Texture2D *uniform)map_Kd;
    self->Kd = Kd;
    self->map_Ks = (uniform Texture2D *uniform)map_Ks;
    self->Ks = Ks;
    self->map_Ns = (uniform Texture2D *uniform)map_Ns;
    self->Ns = Ns;
    self->map_Bump = (uniform Texture2D *uniform)map_Bump;
    self->map_Normal = (uniform Texture2D *uniform)map_Normal;
}
