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

#include "CylindricStereoTrackedCamera.ih"
#include "math/LinearSpace.ih"
#include "math/math.ih"
#include "math/sampling.ih"

static const float uniform OPENDECK_RADIUS = 2.55f;
static const float uniform OPENDECK_HEIGHT = 2.3f;
static const float uniform OPENDECK_METALSTRIPE_HEIGHT = 0.045f;

static const uint8 LEFT_WALL = 0;
static const uint8 RIGHT_WALL = 1;
static const uint8 LEFT_FLOOR = 2;
static const uint8 RIGHT_FLOOR = 3;

void CylindricStereoTrackedCamera_initRay(uniform Camera* uniform _self,
                                          varying Ray& ray,
                                          const varying CameraSample& sample)
{
    uniform CylindricStereoTrackedCamera* uniform self =
        (uniform CylindricStereoTrackedCamera * uniform)_self;

    const vec2f screen = Camera_subRegion(_self, sample.screen);

    float eyeDelta = 0.0f;
    float alpha = 0.0f;
    if (self->bufferId == LEFT_WALL)
    {
        eyeDelta = -self->half_ipd;
        alpha = screen.x * pi;
    }
    else if (self->bufferId == RIGHT_WALL)
    {
        eyeDelta = self->half_ipd;
        alpha = screen.x * pi;
    }
    else if (self->bufferId == LEFT_FLOOR)
    {
        eyeDelta = -self->half_ipd;
    }
    else if (self->bufferId == RIGHT_FLOOR)
    {
        eyeDelta = self->half_ipd;
    }

    vec3f pixelPos;
    if (self->bufferId == LEFT_WALL || self->bufferId == RIGHT_WALL)
    {
        pixelPos.x = self->opendeck_radius * -cosf(alpha);
        // pixelPos.y = OPENDECK_METALSTRIPE_HEIGHT + OPENDECK_HEIGHT *
        // screen.y;
        pixelPos.y = self->opendeck_metal_stripe_height +
                     self->opendeck_height - self->opendeck_height * screen.y;
        pixelPos.z = self->opendeck_radius * -sinf(alpha);
    }
    else if (self->bufferId == LEFT_FLOOR || self->bufferId == RIGHT_FLOOR)
    {
        pixelPos.x = 2.0f * self->opendeck_radius * (screen.x - 0.5f);
        pixelPos.y = 0.0f;
        pixelPos.z = -self->opendeck_radius + self->opendeck_radius * screen.y;
        // pixelPos.z = -OPENDECK_RADIUS * screen.y;
    }

    const vec3f eyeAxis = self->od_dir_du;
    const vec3f eyeDeltaPos = eyeAxis * eyeDelta;

    const LinearSpace3f cameraSpace =
        make_LinearSpace3f(self->dir_du, self->dir_dv, self->dir_cam);

    vec3f dir = normalize(pixelPos - self->headPos + eyeDeltaPos);
    dir = normalize(cameraSpace * dir);

    const vec3f org = self->org + self->headPos - eyeDeltaPos;
    const float time = Camera_shutterTime(_self, sample.time);
    setRay(ray, org, normalize(dir), self->super.nearClip, inf, time);
}

/*! create a new ispc-side version of a OpenDeckCylinderStereoCamera - with
    given pointer to the c-class version - and return both class pointer and
    pointer to internal data back via the supplied reference parameters. */
export void* uniform CylindricStereoTrackedCamera_create(void* uniform cppE)
{
    uniform CylindricStereoTrackedCamera* uniform self =
        uniform new uniform CylindricStereoTrackedCamera;
    self->super.cppEquivalent = cppE;
    self->super.initRay = CylindricStereoTrackedCamera_initRay;
    self->super.doesDOF = false;
    return self;
}

export void CylindricStereoTrackedCamera_set(
    void* uniform _self, const uniform vec3f& org, const uniform vec3f& headPos,
    const uniform vec3f& dir_cam, const uniform vec3f& dir_du,
    const uniform vec3f& dir_dv, const uniform vec3f& od_dir_du,
    const uniform float interpupillaryDistance, const uniform uint8 bufferId,
    const uniform float cameraScaling)
{
    uniform CylindricStereoTrackedCamera* uniform self =
        (uniform CylindricStereoTrackedCamera * uniform)_self;
    self->org = org;
    self->headPos = headPos * cameraScaling;
    self->dir_cam = dir_cam;
    self->dir_du = dir_du;
    self->dir_dv = dir_dv;
    self->od_dir_du = od_dir_du;
    self->half_ipd = interpupillaryDistance * cameraScaling * 0.5f;
    self->bufferId = bufferId;
    self->opendeck_radius = OPENDECK_RADIUS * cameraScaling;
    self->opendeck_height = OPENDECK_HEIGHT * cameraScaling;
    self->opendeck_metal_stripe_height =
        OPENDECK_METALSTRIPE_HEIGHT * cameraScaling;
}
