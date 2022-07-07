/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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
#pragma once

#include <ospray/SDK/modules/cpu/camera/CameraShared.h>

#ifdef __cplusplus
namespace ispc
{
#endif // __cplusplus

struct OsprayCylindricCamera
{
    Camera super;

    vec3f org;
    vec3f dirCam;
    vec3f dirDu;
    vec3f dirDv;
    float imgPlaneSizeY;

#ifdef __cplusplus
    OsprayCylindricCamera()
        : org(0.f)
        , dirCam(0.f)
        , dirDu(0.f)
        , dirDv(0.f)
        , imgPlaneSizeY(0.f)
    {
    }
};
} // namespace ispc
#else
};
#endif // __cplusplus
