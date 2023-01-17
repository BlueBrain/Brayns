/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include "OsprayCylindricCamera.h"
#include "ospray/OsprayCylindricCamera_ispc.h"

#include <rkcommon/math/rkmath.h>

OsprayCylindricCamera::OsprayCylindricCamera()
{
    getSh()->super.initRay = ispc::OsprayCylindricCamera_initRay_addr();
}

std::string OsprayCylindricCamera::toString() const
{
    return "OsprayCylindricCamera";
}

void OsprayCylindricCamera::commit()
{
    ospray::Camera::commit();

    auto fovy = getParam<float>("fovy", 48.549f);

    dir = normalize(dir);
    const auto dirDu = normalize(cross(dir, up));
    const auto dirDv = normalize(up);
    dir = -dir;

    const auto imgPlaneSizeY = 2.0f * tanf(rkcommon::math::deg2rad(0.5f * fovy));

    auto ispcEquivalent = getSh();
    ispcEquivalent->org = pos;
    ispcEquivalent->dirCam = dir;
    ispcEquivalent->dirDu = dirDu;
    ispcEquivalent->dirDv = dirDv;
    ispcEquivalent->imgPlaneSizeY = imgPlaneSizeY;
}
