/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include <brayns/common/Transformation.h>
#include <ospray/SDK/common/OSPCommon.h>

namespace brayns
{
inline osp::affine3f transformationToAffine3f(
    const Transformation& transformation)
{
    ospcommon::affine3f t(ospcommon::one);
    const auto& scale = transformation.getScale();
    t *= t.scale({scale.x(), scale.y(), scale.z()});
    const auto& translation = transformation.getTranslation();
    t *= t.translate({translation.x(), translation.y(), translation.z()});

    const auto& matrix = transformation.getRotation().getRotationMatrix();
    const float x = atan2(matrix(2, 1), matrix(2, 2));
    const float y = atan2(-matrix(2, 0),
                          sqrt(powf(matrix(2, 1), 2) + powf(matrix(2, 2), 2)));
    const float z = atan2(matrix(1, 0), matrix(0, 0));
    if (x != 0.f)
        t *= t.rotate({1, 0, 0}, x);
    if (y != 0.f)
        t *= t.rotate({0, 1, 0}, y);
    if (z != 0.f)
        t *= t.rotate({0, 0, 1}, z);
    return (osp::affine3f&)t;
}
}
