/* Copyright (c) 2020, EPFL/Blue Brain Project
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

#include "Util.h"

#include <brayns/common/Transformation.h>

#include <pbrt/core/transform.h>

namespace brayns
{
pbrt::Transform pbrtTranslation(const Vector3f& v)
{
    pbrt::Matrix4x4 translation (1.f, 0.f, 0.f, v.x,
                                 0.f, 1.f, 0.f, v.y,
                                 0.f, 0.f, 1.f, v.z,
                                 0.f, 0.f, 0.f, 1.f);
    return pbrt::Transform(translation);
}

pbrt::Transform pbrtTransform(const Transformation& t)
{
    //auto m = t.toMatrix();

    const auto& trans = t.getTranslation();
    const auto& quatRot = t.getRotation();
    const auto& scale = t.getScale();

    pbrt::Quaternion a;
    a.v.x = quatRot.x;
    a.v.y = quatRot.y;
    a.v.z = quatRot.z;
    a.w = quatRot.w;

    const auto trRot = a.ToTransform();
    const auto trTrans = pbrt::Translate(pbrt::Vector3f(static_cast<pbrt::Float>(trans.x),
                                                        static_cast<pbrt::Float>(trans.y),
                                                        static_cast<pbrt::Float>(trans.z)));
    const auto trScale = pbrt::Scale(static_cast<pbrt::Float>(scale.x),
                                     static_cast<pbrt::Float>(scale.y),
                                     static_cast<pbrt::Float>(scale.z));

    const auto finalMatrix = pbrt::Matrix4x4::Mul(pbrt::Matrix4x4::Mul(trRot.GetMatrix(),
                                                                       trTrans.GetMatrix()),
                                                  trScale.GetMatrix());

    return pbrt::Transform (finalMatrix);
}
}
