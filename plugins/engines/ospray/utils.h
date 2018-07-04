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
    // https://stackoverflow.com/a/18436193
    const auto& quat = transformation.getRotation();
    const float x = atan2(2 * (quat.w() * quat.x() + quat.y() * quat.z()),
                          1 - 2 * (quat.x() * quat.x() + quat.y() * quat.y()));
    const float y = asin(2 * (quat.w() * quat.y() - quat.z() * quat.x()));
    const float z = atan2(2 * (quat.w() * quat.z() + quat.x() * quat.y()),
                          1 - 2 * (quat.y() * quat.y() + quat.z() * quat.z()));

    ospcommon::affine3f rot{ospcommon::one};
    rot = ospcommon::affine3f::rotate({1, 0, 0}, x) * rot;
    rot = ospcommon::affine3f::rotate({0, 1, 0}, y) * rot;
    rot = ospcommon::affine3f::rotate({0, 0, 1}, z) * rot;

    const auto& translation = transformation.getTranslation();
    const auto& scale = transformation.getScale();

    const auto t =
        ospcommon::affine3f::translate(
            {translation.x(), translation.y(), translation.z()}) *
        rot * ospcommon::affine3f::scale({scale.x(), scale.y(), scale.z()});
    return (osp::affine3f&)t;
}

inline void addInstance(OSPModel rootModel, OSPModel modelToAdd,
                        const Transformation& transform)
{
    OSPGeometry instance =
        ospNewInstance(modelToAdd, transformationToAffine3f(transform));
    ospCommit(instance);
    ospAddGeometry(rootModel, instance);
    ospRelease(instance);
}
}
