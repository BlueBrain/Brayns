/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
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

#include "Cylinder.h"

namespace brayns
{
Cylinder::Cylinder(const Vector3f& center, const Vector3f& up,
                   const float radius, const float timestamp,
                   const Vector2f& values)
    : Primitive(timestamp, values)
    , _center(center)
    , _up(up)
    , _radius(radius)
{
    _geometryType = GT_CYLINDER;
}

size_t Cylinder::serializeData(floats& serializedData)
{
    serializedData.push_back(_center.x());
    serializedData.push_back(_center.y());
    serializedData.push_back(_center.z());
    serializedData.push_back(_up.x());
    serializedData.push_back(_up.y());
    serializedData.push_back(_up.z());
    serializedData.push_back(_radius);
    serializedData.push_back(_timestamp);
    serializedData.push_back(_values.x());
    serializedData.push_back(_values.y());
    return getSerializationSize();
}

size_t Cylinder::getSerializationSize()
{
    return 10;
}
}
