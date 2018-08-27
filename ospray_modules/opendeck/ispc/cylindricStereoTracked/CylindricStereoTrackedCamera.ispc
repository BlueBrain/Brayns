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

#include "CylindricStereoTrackedCamera.ih"
#include "math/LinearSpace.ih"
#include "math/math.ih"
#include "math/sampling.ih"

void CylindricStereoTrackedCamera_initRay(uniform Camera *uniform _self, varying Ray &ray,
                                          const varying CameraSample &sample)
{
    uniform CylindricStereoTrackedCamera *uniform self =
        (uniform CylindricStereoTrackedCamera * uniform)_self;

    vec2f screen = sample.screen;
    screen = Camera_subRegion(_self, screen);
    vec3f org;
    vec3f dir;
    float eyeDelta = 0;
    float alpha;
    if( self->side_by_side )
    {
        if(screen.x < 0.5)
        {
            eyeDelta = -self->interpupillaryDistance * 0.5f;
            alpha = 2.0f * screen.x * pi;
        }
        else
        {
            eyeDelta = self->interpupillaryDistance * 0.5f;
            alpha = 2.0f * (screen.x - 0.5f) * pi;
        }
    }
    else
    {
        alpha = screen.x * pi;
    }

    vec3f pixelPos;
    pixelPos.x = self->openDeckRadius * -cosf(alpha);
    pixelPos.y = self->openDeckVerticalSize * screen.y;
    pixelPos.z = self->openDeckRadius * -sinf(alpha);

    vec3f eyeAxis = self->od_dir_du;/*normalize(cameraSpace * self->od_dir_du);*/
    vec3f eyeDeltaPos = eyeAxis * eyeDelta;  

    dir = normalize( pixelPos - self->openDeckOrg + eyeDeltaPos);

    const LinearSpace3f cameraSpace = make_LinearSpace3f( self->dir_du, self->dir_dv, self->dir_cam);
    dir = normalize( cameraSpace * dir );

    org.x = self->org.x + self->openDeckOrg.x - eyeDeltaPos.x;
    org.y = self->org.y + self->openDeckOrg.y - eyeDeltaPos.y;
    org.z = self->org.z + self->openDeckOrg.z - eyeDeltaPos.z;

    const float time = Camera_shutterTime(_self, sample.time);
    setRay(ray, org, normalize(dir), self->super.nearClip, infinity, time);
}

/*! create a new ispc-side version of a OpenDeckCylinderStereoCamera - with given
    pointer to the c-class version - and return both class pointer and
    pointer to internal data back via the supplied reference
    parameters */
export void *uniform CylindricStereoTrackedCamera_create(void *uniform cppE)
{
    uniform CylindricStereoTrackedCamera *uniform self = uniform new uniform CylindricStereoTrackedCamera;
    self->super.cppEquivalent = cppE;
    self->super.initRay = CylindricStereoTrackedCamera_initRay;
    self->super.doesDOF = false;
    return self;
}

export void CylindricStereoTrackedCamera_set(
    void *uniform _self, const uniform vec3f &org, const uniform vec3f &openDeckOrg, const uniform vec3f &dir_cam,
    const uniform vec3f &dir_du, const uniform vec3f &dir_dv, const uniform vec3f &od_dir_du, 
    const uniform float openDeckVerticalSize, const float uniform openDeckRadius,
    const uniform float interpupillaryDistance, const uniform bool side_by_side)
{
    uniform CylindricStereoTrackedCamera *uniform self =
        (uniform CylindricStereoTrackedCamera * uniform)_self;
    self->org = org;
    self->openDeckOrg = openDeckOrg;
    self->dir_cam = dir_cam;
    self->dir_du = dir_du;
    self->dir_dv = dir_dv;
    self->od_dir_du = od_dir_du;
    self->openDeckVerticalSize = openDeckVerticalSize;
    self->openDeckRadius = openDeckRadius;
    self->interpupillaryDistance = interpupillaryDistance;
    self->side_by_side = side_by_side;
}
