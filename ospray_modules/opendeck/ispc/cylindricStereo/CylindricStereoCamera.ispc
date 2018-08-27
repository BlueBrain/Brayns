/* Copyright (c) 2018, EPFL/Blue Brain Project
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

#include "CylindricStereoCamera.ih"
#include "math/LinearSpace.ih"
#include "math/math.ih"
#include "math/sampling.ih"

const float openDeckRadius = 2.55f;

void CylindricStereoCamera_initRay(uniform Camera *uniform _self, varying Ray &ray,
                                   const varying CameraSample &sample)
{
    uniform CylindricStereoCamera *uniform self =
        (uniform CylindricStereoCamera * uniform)_self;

    vec2f screen = sample.screen;
    screen = Camera_subRegion(_self, screen);
    vec3f org;
    vec3f dir;
    float radius;
    float alpha;
    float beta = atan(self->interpupillaryDistance / (2.0f * openDeckRadius));
    if( self->side_by_side )
    {
        if(screen.x < 0.5)
        {
            radius = self->interpupillaryDistance * 0.5f;
            alpha = 2.0f * screen.x * -pi;
            beta = -beta;
        }
        else
        {
            radius = -self->interpupillaryDistance * 0.5f;
            alpha = 2.0f * (screen.x - 0.5f) * -pi;
        }
    }
    else
    {
        radius = self->interpupillaryDistance * 0.5f;
        alpha = screen.x * -pi;
    }

    const float y = self->imgPlane_size_y * (screen.y - 0.5f);
    dir = normalize(make_vec3f(sinf(alpha + beta), y, -cosf(alpha + beta)));

    const LinearSpace3f cameraSpace = make_LinearSpace3f( self->dir_cam, self->dir_dv, self->dir_du);
    dir = normalize( cameraSpace * dir );

    org.x = self->org.x + self->dir_cam.x * radius * cosf(alpha) + self->dir_du.x * radius * sinf(alpha);
    org.y = self->org.y + self->dir_cam.y * radius * cosf(alpha) + self->dir_du.y * radius * sinf(alpha);
    org.z = self->org.z + self->dir_cam.z * radius * cosf(alpha) + self->dir_du.z * radius * sinf(alpha);

    const float time = Camera_shutterTime(_self, sample.time);
    setRay(ray, org, normalize(dir), self->super.nearClip, infinity, time);
}

/*! create a new ispc-side version of a OpenDeckCylinderStereoCamera - with given
    pointer to the c-class version - and return both class pointer and
    pointer to internal data back via the supplied reference
    parameters */
export void *uniform CylindricStereoCamera_create(void *uniform cppE)
{
    uniform CylindricStereoCamera *uniform self = uniform new uniform CylindricStereoCamera;
    self->super.cppEquivalent = cppE;
    self->super.initRay = CylindricStereoCamera_initRay;
    self->super.doesDOF = false;
    return self;
}

export void CylindricStereoCamera_set(
    void *uniform _self, const uniform vec3f &org, const uniform vec3f &dir_cam,
    const uniform vec3f &dir_du, const uniform vec3f &dir_dv, const uniform float imgPlane_size_y,
    const uniform float interpupillaryDistance, const uniform bool side_by_side)
{
    uniform CylindricStereoCamera *uniform self =
        (uniform CylindricStereoCamera * uniform)_self;
    self->org = org;
    self->dir_cam = dir_cam;
    self->dir_du = dir_du;
    self->dir_dv = dir_dv;
    self->interpupillaryDistance = interpupillaryDistance;
    self->imgPlane_size_y = imgPlane_size_y;
    self->side_by_side = side_by_side;
}
