/* Copyright 2018-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
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

#include "PerspectiveParallaxCamera.h"
#include "PerspectiveParallaxCamera_ispc.h"

namespace ospray
{
PerspectiveParallaxCamera::PerspectiveParallaxCamera()
{
    ispcEquivalent = ispc::PerspectiveParallaxCamera_create(this);
}

void PerspectiveParallaxCamera::commit()
{
    Camera::commit();

    const float fovy = getParamf("fovy", 60.f);
    float aspectRatio = getParamf("aspect", 1.5f);

    const float interpupillaryDistance =
        getParamf("interpupillaryDistance", 0.0635f);
    const float zeroParallaxPlane = getParamf("zeroParallaxPlane", 1.f);

    float idpOffset = 0.0f;
    auto bufferTarget = getParamString("buffer_target");
    if (bufferTarget.length() == 2)
    {
        if (bufferTarget.at(1) == 'L')
            idpOffset = -interpupillaryDistance * 0.5f;
        if (bufferTarget.at(1) == 'R')
            idpOffset = +interpupillaryDistance * 0.5f;
    }

    vec3f org = pos;
    dir = normalize(dir);
    const vec3f dir_du = normalize(cross(dir, up));
    const vec3f dir_dv = normalize(up);
    dir = -dir;

    const float imgPlane_size_y =
        2.f * zeroParallaxPlane * tanf(deg2rad(0.5f * fovy));
    const float imgPlane_size_x = imgPlane_size_y * aspectRatio;

    ispc::PerspectiveParallaxCamera_set(getIE(), (const ispc::vec3f&)org,
                                        (const ispc::vec3f&)dir,
                                        (const ispc::vec3f&)dir_du,
                                        (const ispc::vec3f&)dir_dv,
                                        zeroParallaxPlane, imgPlane_size_y,
                                        imgPlane_size_x, idpOffset);
}

OSP_REGISTER_CAMERA(PerspectiveParallaxCamera, perspectiveParallax);

} // namespace ospray
