/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#include "Sphere.h"

namespace brayns
{

Sphere::Sphere(
    const size_t materialId,
    const Vector3f& center,
    const float radius,
    const float timestamp,
    const float value)
    : Primitive(materialId)
    , _center(center)
    , _radius(radius)
    , _timestamp(timestamp)
    , _value(value)
{
    _geometryType = GT_SPHERE;
}

size_t Sphere::serializeData(floats& serializedData)
{
    serializedData.push_back(_center.x());
    serializedData.push_back(_center.y());
    serializedData.push_back(_center.z());
    serializedData.push_back(_radius);
    serializedData.push_back(_timestamp);
    serializedData.push_back(_value);
    return getSerializationSize();
}

size_t Sphere::getSerializationSize()
{
    return 6;
}

}
