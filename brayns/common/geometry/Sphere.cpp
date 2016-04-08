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
    const float timestamp)
    : Primitive(materialId)
    , _center(center)
    , _radius(radius)
    , _timestamp(timestamp)
{
    _geometryType = GT_SPHERE;
}

size_t Sphere::serializeData(floats& serializedData)
{
    floats data;
    data.push_back(_center.x());
    data.push_back(_center.y());
    data.push_back(_center.z());
    data.push_back(_radius);
    data.push_back(_timestamp);
    serializedData.insert(std::end(serializedData), std::begin(data), std::end(data));
    return data.size();
}

}
