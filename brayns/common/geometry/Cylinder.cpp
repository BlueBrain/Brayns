/* Copyright (c) 2011-2015, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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

Cylinder::Cylinder(
    const size_t materialId,
    const Vector3f& center,
    const Vector3f& up,
    const float radius,
    const float timestamp)
    : Primitive(materialId)
    , _center(center)
    , _up(up)
    , _radius(radius)
    , _timestamp(timestamp)
{
    _geometryType = GT_CYLINDER;
}

size_t Cylinder::serializeData(floats& serializedData)
{
    floats data;
    data.push_back(_center.x());
    data.push_back(_center.y());
    data.push_back(_center.z());
    data.push_back(_up.x() );
    data.push_back(_up.y() );
    data.push_back(_up.z() );
    data.push_back(_radius );
    data.push_back(_timestamp );
    serializedData.insert(std::end(serializedData), std::begin(data), std::end(data));
    return data.size();
}

}
