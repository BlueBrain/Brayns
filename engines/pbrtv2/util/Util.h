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

#ifndef UTIL_H
#define UTIL_H

#include <brayns/common/mathTypes.h>
#include <brayns/common/types.h>

#include <pbrtv2/core/geometry.h>

namespace brayns
{
template<class T>
T glmToPbrt3(const glm::vec3& p)
{
    return T(
        static_cast<float>(p.x),
        static_cast<float>(p.y),
        static_cast<float>(p.z));
}

template<class T>
glm::vec3 pbrtToGlm3(const T& p)
{
    return glm::vec3(p.x, p.y, p.z);
}

pbrt::Transform pbrtTranslation(const Vector3f& v);

pbrt::Transform pbrtTransform(const Transformation& t);

}

#endif
