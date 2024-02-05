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

struct PanoramicCamera
{
    Camera super;

    vec3f org;        //!< position of camera, already contains shift when
                      //! STEREO_{LEFT|RIGHT}
    vec3f ipd_offset; //!< shift of camera position for left/right eye (only
                      //! when stereo)
    vec3f dirCam;

    vec3f dirU;

    vec3f dirV;

    // Clip planes
    const uniform vec4f* clipPlanes;
    unsigned int numClipPlanes;

    bool half;
};

void PanoramicCamera_initRay(uniform Camera* uniform _self, varying Ray& ray,
                             const varying CameraSample& sample)
{
    uniform PanoramicCamera* uniform self =
        (uniform PanoramicCamera * uniform) _self;

    vec2f screen = sample.screen;
    const vec3f org = self->org;

    screen = Camera_subRegion(_self, screen);

    const float phi =
        self->half ? M_PI / 2.f - two_pi * screen.x * 0.5f : two_pi * screen.x;
    const float theta = M_PI * screen.y;

    float sinTheta, cosTheta;
    sincos(theta, &sinTheta, &cosTheta);
    const vec3f localDir = cartesian(phi, sinTheta, cosTheta);

    // trnasform to camera- and then to world-space
    vec3f dir = make_vec3f(-localDir.y, -localDir.z, localDir.x);

    const LinearSpace3f cameraSpace =
        make_LinearSpace3f(self->dirU, self->dirV, self->dirCam);

    dir = normalize(cameraSpace * dir);

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
export void* uniform PanoramicCamera_create(void* uniform cppE)
{
    uniform PanoramicCamera* uniform self = uniform new uniform PanoramicCamera;
    self->super.cppEquivalent = cppE;
    self->super.initRay = PanoramicCamera_initRay;
    self->super.doesDOF = false;
    self->numClipPlanes = 0;
    return self;
}

export void PanoramicCamera_set(
    void* uniform _self, const uniform vec3f& org, const uniform vec3f& dir,
    const uniform vec3f& dirU, const uniform vec3f& dirV,
    const uniform vec3f& ipd_offset, const uniform vec4f clipPlanes[],
    const uniform unsigned int numClipPlanes, const uniform bool& half)
{
    uniform PanoramicCamera* uniform self =
        (uniform PanoramicCamera * uniform) _self;
    self->org = org;
    self->dirCam = dir;
    self->dirU = dirU;
    self->dirV = dirV;
    self->ipd_offset = ipd_offset;
    self->clipPlanes = clipPlanes;
    self->numClipPlanes = numClipPlanes;
    self->half = half;
}
