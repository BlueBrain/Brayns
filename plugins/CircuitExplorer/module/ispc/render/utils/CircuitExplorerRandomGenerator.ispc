/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * <https://github.com/favreau/Brayns-UC-CircuitExplorer>
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

#include "CircuitExplorerRandomGenerator.ih"

#define HIGH_QUALITY_RANDOM

uniform bool seedInitialized = false;
struct RNGState rngState;

inline float rotate(float x, const float dx)
{
    x += dx;
    if (x >= 1.f)
        x -= 1.f;
    return x;
}

inline float getRandomValue(const varying ScreenSample&, const int)
{
    if (!seedInitialized && programIndex == 0)
    {
        float seed;
        rdrand(&seed);
        seed_rng(&rngState, seed);
        seedInitialized = true;
    }

    float r = 0.f;
#ifdef HIGH_QUALITY_RANDOM
    int nbMaxTries = 10;
    while (nbMaxTries >= 0 && rdrand(&r) == false)
        --nbMaxTries;
#else
    rdrand(&r);
#endif
    return r;
}

void getTangentVectors(const vec3f& normal, vec3f& tangent, vec3f& biTangent)
{
    tangent = make_vec3f(1.f, 0.f, 0.f);
    if (abs(dot(tangent, normal)) > 0.95f)
        tangent = make_vec3f(0.f, 1.f, 0.f);
    biTangent = normalize(cross(tangent, normal));
    tangent = normalize(cross(biTangent, normal));
}

inline vec3f getRandomVector(const unsigned int /*frameBufferWidth*/,
                             const varying ScreenSample& sample,
                             const vec3f& normal, const int randomNumber)
{
    const float rx = getRandomValue(sample, randomNumber) - 0.5f;
    const float ry = getRandomValue(sample, randomNumber) - 0.5f;
    const float rz = getRandomValue(sample, randomNumber) - 0.5f;
    return normalize(normal + make_vec3f(rx, ry, rz));
}

vec3f getRandomDir(varying RandomTEA* uniform rng, const vec3f biNorm0,
                   const vec3f biNorm1, const vec3f gNormal, const float rot_x,
                   const float rot_y, const uniform float epsilon)
{
    const vec2f rn = RandomTEA__getFloats(rng);
    const float r0 = rotate(rn.x, rot_x);
    const float r1 = rotate(rn.y, rot_y);

    const float w = sqrt(1.f - r1);
    const float x = cos((2.f * M_PI) * r0) * w;
    const float y = sin((2.f * M_PI) * r0) * w;
    const float z = sqrt(r1) + epsilon;
    return x * biNorm0 + y * biNorm1 + z * gNormal;
}

//  http://lolengine.net/blog/2013/09/21/picking-orthogonal-vector-combing-coconuts
vec3f ortho(const vec3f& v)
{
    return abs(v.x) > abs(v.z) ? make_vec3f(-v.y, v.x, 0.0f)
                               : make_vec3f(0.0f, -v.z, v.y);
}

vec3f getConeSample(const vec3f& direction, varying RandomTEA* uniform rng,
                    const float extent)
{
    const vec2f s = RandomTEA__getFloats(rng);
    // Formula 34 in GI Compendium

    const vec3f o1 = normalize(ortho(direction));
    const vec3f o2 = normalize(cross(direction, o1));
    const float phi = s.x * 2.f * M_PI;
    const float cosTheta = 1.f - s.y * extent;
    const float sinTheta =
        sqrtf(1.f - cosTheta * cosTheta); // sin^2 = 1 - cos^2
    return cosf(phi) * sinTheta * o1 + sinf(phi) * sinTheta * o2 +
           cosTheta * direction;
}

varying RandomTEA* uniform createRandomNumberGenerator(
    const uniform Renderer* uniform self, const varying ScreenSample& sample)
{
    // create and seed random number generator (RNG)
    uniform FrameBuffer* uniform fb = self->fb;
    RandomTEA rng_state;
    varying RandomTEA* uniform rng = &rng_state;
    const int accumID = reduce_max(sample.sampleID.z) * self->spp;
    RandomTEA__Constructor(rng,
                           (fb->size.x * sample.sampleID.y) + sample.sampleID.x,
                           accumID);
    return rng;
}

inline float mix(const float x, const float y, const float a)
{
    return x * (1.f - a) + y * a;
}

inline float hash(float n)
{
    return frac(sin(n + 1.951f) * 43758.5453f);
}

float noise(const vec3f& x)
{
    // hash based 3d value noise
    vec3f p = floor(x);
    vec3f f = frac(x);

    f = f * f * (make_vec3f(3.0f) - make_vec3f(2.0f) * f);
    float n = p.x + p.y * 57.0f + 113.0f * p.z;
    return mix(mix(mix(hash(n + 0.0f), hash(n + 1.0f), f.x),
                   mix(hash(n + 57.0f), hash(n + 58.0f), f.x), f.y),
               mix(mix(hash(n + 113.0f), hash(n + 114.0f), f.x),
                   mix(hash(n + 170.0f), hash(n + 171.0f), f.x), f.y),
               f.z);
}

inline vec3f mod(const vec3f& v, const int m)
{
    return make_vec3f(v.x - m * floor(v.x / m), v.y - m * floor(v.y / m),
                      v.z - m * floor(v.z / m));
}

float cells(const vec3f& p, float cellCount)
{
    const vec3f pCell = p * cellCount;
    float d = 1.0e10;
    for (int xo = -1; xo <= 1; xo++)
    {
        for (int yo = -1; yo <= 1; yo++)
        {
            for (int zo = -1; zo <= 1; zo++)
            {
                vec3f tp = floor(pCell) + make_vec3f(xo, yo, zo);

                tp = pCell - tp - noise(mod(tp, cellCount / 1));

                d = min(d, dot(tp, tp));
            }
        }
    }
    d = min(d, 1.0f);
    d = max(d, 0.0f);
    return d;
}

float worleyNoise(const vec3f& p, float cellCount)
{
    return cells(p, cellCount);
}
