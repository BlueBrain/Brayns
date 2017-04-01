/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include <ospray/SDK/render/Renderer.ih>
#include <ospray/SDK/render/util.ih>

#include <plugins/engines/ospray/ispc/render/utils/RandomGenerator.ih>

uniform bool seedInitialized = false;
struct RNGState rngState;

#define HIGH_QUALITY_RANDOM

inline float getRandomValue()
{
    float r;
#ifdef HIGH_QUALITY_RANDOM
    while (rdrand(&r) == false)
    {
    }
#else
    rdrand(&r);
#endif
    return r;
}

inline vec3f getRandomVector(const vec3f& normal)
{
    if (!seedInitialized)
    {
        seed_rng(&rngState, programIndex);
        seedInitialized = true;
    }

    const float rx = getRandomValue() - 0.5f;
    const float ry = getRandomValue() - 0.5f;
    const float rz = getRandomValue() - 0.5f;
    return normalize(normal + make_vec3f(rx, ry, rz));
}

void getTangentVectors(const vec3f& normal, vec3f& tangent, vec3f& biTangent)
{
    tangent = make_vec3f(1.f, 0.f, 0.f);
    if (abs(dot(tangent, normal)) > 0.95f)
        tangent = make_vec3f(0.f, 1.f, 0.f);
    biTangent = normalize(cross(tangent, normal));
    tangent = normalize(cross(biTangent, normal));
}
