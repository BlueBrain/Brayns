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

#include "Cone.h"

namespace brayns
{
Cone::Cone(const Vector3f& center, const Vector3f& up, const float centerRadius,
           const float upRadius, const float timestamp, const float value)
    : Primitive(timestamp)
    , _center(center)
    , _up(up)
    , _centerRadius(centerRadius)
    , _upRadius(upRadius)
    , _value(value)
{
    _geometryType = GT_CONE;
}

size_t Cone::serializeData(floats& serializedData)
{
    serializedData.push_back(_center.x());
    serializedData.push_back(_center.y());
    serializedData.push_back(_center.z());
    serializedData.push_back(_up.x());
    serializedData.push_back(_up.y());
    serializedData.push_back(_up.z());
    serializedData.push_back(_centerRadius);
    serializedData.push_back(_upRadius);
    serializedData.push_back(_timestamp);
    serializedData.push_back(_value);
    return getSerializationSize();
}

size_t Cone::getSerializationSize()
{
    return 10;
}
}
