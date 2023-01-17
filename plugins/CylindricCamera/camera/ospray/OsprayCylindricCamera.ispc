/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
 *                     Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>      
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

#include "ospray/SDK/modules/cpu/camera/Camera.ih"
#include "ospray/SDK/modules/cpu/math/LinearSpace.ih"

#include "rkcommon/math/math.ih"

#include "OsprayCylindricCameraShared.h"

void OsprayCylindricCamera_initRay(const Camera *uniform _self, varying Ray &ray, const varying CameraSample &sample)
{
    const OsprayCylindricCamera *uniform self = (const OsprayCylindricCamera *uniform)_self;

    vec2f screen = sample.screen;
    screen = Camera_subRegion(_self, screen);

    const float alpha = screen.x * -pi;
    const float y = self->imgPlaneSizeY * (screen.y - 0.5f);
    vec3f dir = normalize(make_vec3f(-cosf(alpha), y, sinf(alpha)));
    
    const LinearSpace3f cameraSpace = make_LinearSpace3f(self->dirDu, self->dirDv, self->dirCam);
    dir = normalize(cameraSpace * dir);

    const float time = Camera_shutterTime(_self, screen, sample.time);
    vec3f org = self->org;
    setRay(ray, org, normalize(dir), self->super.nearClip, inf, time);
}

export void *uniform OsprayCylindricCamera_initRay_addr()
{
    return OsprayCylindricCamera_initRay;
}
