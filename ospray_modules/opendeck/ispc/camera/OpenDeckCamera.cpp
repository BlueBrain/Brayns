/* Copyright (c) 2018, Cyrille Favreau
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille_favreau@hotmail.com>
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

#include "OpenDeckCamera.h"
#include "OpenDeckCamera_ispc.h"

namespace ospray
{
OpenDeckCamera::OpenDeckCamera()
{
    ispcEquivalent = ispc::OpenDeckCamera_create(this);
}

std::string OpenDeckCamera::toString() const
{
    return "ospray::OpenDeckCamera";
}

void OpenDeckCamera::commit()
{
    Camera::commit();

    const float fovy = getParamf("fovy", 60.f);

    dir = normalize(dir);
    const vec3f dir_du = normalize(cross(dir, up));
    const vec3f dir_dv = normalize(up);
    dir = -dir;

    const vec3f org = pos;
    const float imgPlane_size_y = 2.0f * tanf(deg2rad(0.5f * fovy));

    ispc::OpenDeckCamera_set(getIE(), (const ispc::vec3f&)org,
                             (const ispc::vec3f&)dir,
                             (const ispc::vec3f&)dir_du,
                             (const ispc::vec3f&)dir_dv,
                             imgPlane_size_y);
}

OSP_REGISTER_CAMERA(OpenDeckCamera, opendeck);

} // ::ospray
