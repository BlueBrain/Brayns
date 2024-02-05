/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
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

#include "utils.ih"

#include "math/sampling.ih"
#include "ospray/SDK/camera/Camera.ih"

struct FishEyeCamera
{
    Camera super;

    vec3f org; //!< position of camera, already contains shift when
               //! STEREO_{LEFT|RIGHT}
    vec3f dirCam;

    vec3f dirU;

    vec3f dirV;

    // Clip planes
    const uniform vec4f* clipPlanes;
    unsigned int numClipPlanes;

    float aperture;
};

inline float getRandomValue()
{
    float r;
    int nbMaxTries = 10;
    while (nbMaxTries >= 0 && rdrand(&r) == false)
        --nbMaxTries;
    return r;
}

void FishEyeCamera_initRay(uniform Camera* uniform _self, varying Ray& ray,
                           const varying CameraSample& sample)
{
    uniform FishEyeCamera* uniform self =
        (uniform FishEyeCamera * uniform) _self;

    vec2f screen = sample.screen;
    vec3f org = self->org;

    screen = Camera_subRegion(_self, screen);

    float xp = screen.x * 2.f - 1.f;
    xp = (xp == 0.f ? 0.0001f : xp);

    const float yp = screen.y * 2.f - 1.f;
    const float radius = sqrt(xp * xp + yp * yp);
    vec3f dir = make_vec3f(0.f);
    if (radius > 1.05f) // Render 5% larger than required for safety reason :)
    {
        setRay(ray, org, dir, 0.f, 0.f, 0.f);
        return;
    }

    const float phi = (radius - 1.f) * PI * 0.5f;
    const float theta = atan2(yp, xp);
    const vec3f localDir = {cos(phi) * cos(theta), cos(phi) * sin(theta),
                            sin(phi)};

    // transform to camera and then to world-space
    dir = normalize(localDir);

    const LinearSpace3f cameraSpace =
        make_LinearSpace3f(self->dirU, self->dirV, self->dirCam);

    dir = normalize(cameraSpace * dir);

    if (true)
    {
        const float scaledAperture = 0.1f;
        float random = 1.f;
        random = getRandomValue();
        random = (random == 0.f ? 0.001f : random);
        const vec3f llp =
            uniformSampleDisk(random * self->aperture, sample.lens);
        // transform local lens point to focal plane (dir_XX are prescaled in
        // this case)
        const vec3f lp = (llp.x * self->dirU) + (llp.y * self->dirV);
        org = org + lp;
        dir = dir - lp;
    }

    float nearClip = self->super.nearClip;
    float farClip = inf;

    clipRay(self->clipPlanes, self->numClipPlanes, org, dir, nearClip, farClip);

    const float time = Camera_shutterTime(_self, sample.time);
    setRay(ray, org, dir, nearClip, farClip, time);
}

/*! create a new ispc-side version of a PerspectiveCamera - with given
    pointer to the c-class version - and return both class pointer and
    pointer to internal data back via the supplied reference
    parameters */
export void* uniform FishEyeCamera_create(void* uniform cppE)
{
    uniform FishEyeCamera* uniform self = uniform new uniform FishEyeCamera;
    self->super.cppEquivalent = cppE;
    self->super.initRay = FishEyeCamera_initRay;
    self->super.doesDOF = false;
    self->numClipPlanes = 0;
    return self;
}

export void FishEyeCamera_set(void* uniform _self, const uniform vec3f& org,
                              const uniform vec3f& dir,
                              const uniform vec3f& dirU,
                              const uniform vec3f& dirV,
                              const uniform vec4f clipPlanes[],
                              const uniform unsigned int numClipPlanes,
                              const uniform float aperture)
{
    uniform FishEyeCamera* uniform self =
        (uniform FishEyeCamera * uniform) _self;
    self->org = org;
    self->dirCam = dir;
    self->dirU = dirU;
    self->dirV = dirV;
    self->clipPlanes = clipPlanes;
    self->numClipPlanes = numClipPlanes;
    self->aperture = aperture;
}
