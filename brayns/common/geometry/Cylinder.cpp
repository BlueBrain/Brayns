/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#include "Cylinder.h"

namespace brayns
{

Cylinder::Cylinder(
    const size_t materialId,
    const Vector3f& center,
    const Vector3f& up,
    const float radius,
    const float timestamp,
    const float value)
    : Primitive(materialId)
    , _center(center)
    , _up(up)
    , _radius(radius)
    , _timestamp(timestamp)
    , _value(value)
{
    _geometryType = GT_CYLINDER;
}

size_t Cylinder::serializeData(floats& serializedData)
{
    serializedData.push_back(_center.x());
    serializedData.push_back(_center.y());
    serializedData.push_back(_center.z());
    serializedData.push_back(_up.x() );
    serializedData.push_back(_up.y() );
    serializedData.push_back(_up.z() );
    serializedData.push_back(_radius );
    serializedData.push_back(_timestamp );
    serializedData.push_back(_value);
    return getSerializationSize();
}

size_t Cylinder::getSerializationSize()
{
    return 9;
}

}
