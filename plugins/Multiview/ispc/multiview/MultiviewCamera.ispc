/* Copyright 2018-2024 Blue Brain Project/EPFL
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

#include "MultiviewCamera.ih"
#include "engines/ospray/ispc/camera/utils.ih"
#include "math/LinearSpace.ih"
#include "math/math.ih"
#include "math/sampling.ih"

void MultiviewCamera_initRay(uniform Camera* uniform _self, varying Ray& ray,
                             const varying CameraSample& sample)
{
    uniform MultiviewCamera* uniform self =
        (uniform MultiviewCamera * uniform)_self;

    vec2f screen = Camera_subRegion(_self, sample.screen);

    if (screen.x < 0.5f && screen.y < 0.5f)
    {
        // left
        const vec3f dir = make_vec3f(1.0f, 0.0f, 0.0f);
        const vec3f pos_du =
            make_vec3f(0.0f, 0.0f, 1.0f) * self->height * self->aspect;
        const vec3f pos_dv = make_vec3f(0.0f, 1.0f, 0.0f) * self->height;
        const vec3f pos_00 =
            (self->org + make_vec3f(-self->armLength, 0.0f, 0.0f)) -
            0.5f * pos_du - 0.5f * pos_dv;

        const vec3f org =
            pos_00 + 2.0 * screen.x * pos_du + 2.0 * screen.y * pos_dv;

        float nearClip = self->super.nearClip;
        float farClip = inf;
        const varying vec3f direction = normalize(dir);

        clipRay(self->clipPlanes, self->numClipPlanes, org, direction, nearClip,
                farClip);

        const float time = Camera_shutterTime(_self, sample.time);
        setRay(ray, org, dir, nearClip, farClip, time);
    }
    else if (screen.x < 0.5f && screen.y >= 0.5f)
    {
        // front
        const vec3f dir = make_vec3f(0.0f, 0.0f, 1.0f);
        const vec3f pos_du =
            make_vec3f(-1.0f, 0.0f, 0.0f) * self->height * self->aspect;
        const vec3f pos_dv = make_vec3f(0.0f, 1.0f, 0.0f) * self->height;
        const vec3f pos_00 =
            (self->org + make_vec3f(0.0f, 0.0f, -self->armLength)) -
            0.5f * pos_du - 0.5f * pos_dv;

        const vec3f org =
            pos_00 + 2.0 * screen.x * pos_du + 2.0 * (screen.y - 0.5f) * pos_dv;

        float nearClip = self->super.nearClip;
        float farClip = inf;
        const varying vec3f direction = normalize(dir);

        clipRay(self->clipPlanes, self->numClipPlanes, org, direction, nearClip,
                farClip);

        const float time = Camera_shutterTime(_self, sample.time);
        setRay(ray, org, dir, nearClip, farClip, time);
    }
    else if (screen.x >= 0.5f && screen.y < 0.5f)
    {
        vec3f org = self->org;

        vec3f dir = self->dir_00 + 2.0f * (screen.x - 0.5f) * self->dir_du +
                    2.0f * screen.y * self->dir_dv;

        if (self->super.doesDOF)
        {
            const vec3f llp =
                uniformSampleDisk(self->scaledAperture, sample.lens);
            // transform local lens point to focal plane (dir_XX are prescaled
            // in
            // this case)
            const vec3f lp = (llp.x * self->dir_du) +
                             ((llp.y * self->aspect) * self->dir_dv);
            org = org + lp;
            dir = dir - lp;
        }

        float nearClip = self->super.nearClip;
        float farClip = inf;
        const varying vec3f direction = normalize(dir);

        clipRay(self->clipPlanes, self->numClipPlanes, org, direction, nearClip,
                farClip);

        const float time = Camera_shutterTime(_self, sample.time);
        setRay(ray, org, direction, nearClip, farClip, time);
    }
    else if (screen.x >= 0.5 && screen.y >= 0.5)
    {
        // top
        const vec3f dir = make_vec3f(0.0f, -1.0f, 0.0f);
        const vec3f pos_du =
            make_vec3f(1.0f, 0.0f, 0.0f) * self->height * self->aspect;
        const vec3f pos_dv = make_vec3f(0.0f, 0.0f, -1.0f) * self->height;
        const vec3f pos_00 =
            (self->org + make_vec3f(0.0f, self->armLength, 0.0f)) -
            0.5f * pos_du - 0.5f * pos_dv;

        const vec3f org = pos_00 + 2.0 * (screen.x - 0.5f) * pos_du +
                          2.0 * (screen.y - 0.5f) * pos_dv;

        float nearClip = self->super.nearClip;
        float farClip = inf;
        const varying vec3f direction = normalize(dir);

        clipRay(self->clipPlanes, self->numClipPlanes, org, direction, nearClip,
                farClip);

        const float time = Camera_shutterTime(_self, sample.time);
        setRay(ray, org, dir, nearClip, farClip, time);
    }
}

/*! create a new ispc-side version of a Multiview camera - with given pointer to
    the c-class version - and return both class pointer and pointer to internal
    data back via the supplied reference parameters. */
export void* uniform MultiviewCamera_create(void* uniform cppE)
{
    uniform MultiviewCamera* uniform self = uniform new uniform MultiviewCamera;
    self->super.cppEquivalent = cppE;
    self->super.initRay = MultiviewCamera_initRay;
    self->super.doesDOF = false;
    return self;
}

export void MultiviewCamera_set(
    void* uniform _self, const uniform vec3f& org, const uniform vec3f& dir_00,
    const uniform vec3f& dir_du, const uniform vec3f& dir_dv,
    const uniform float scaledAperture, const uniform float height,
    const uniform float aspect, const uniform float armLength,
    const uniform vec4f clipPlanes[], const uniform unsigned int numClipPlanes)
{
    uniform MultiviewCamera* uniform self =
        (uniform MultiviewCamera * uniform)_self;
    self->org = org;
    self->dir_00 = dir_00;
    self->dir_du = dir_du;
    self->dir_dv = dir_dv;
    self->scaledAperture = scaledAperture;
    self->super.doesDOF = scaledAperture > 0.f;
    self->aspect = aspect;
    self->height = height;
    self->armLength = armLength;
    self->clipPlanes = clipPlanes;
    self->numClipPlanes = numClipPlanes;
}
