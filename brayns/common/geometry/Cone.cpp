/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#include "Cone.h"

namespace brayns
{

Cone::Cone(
    const size_t materialId,
    const Vector3f& center,
    const Vector3f& up,
    const float centerRadius,
    const float upRadius,
    const float timestamp,
    const float value)
    : Primitive(materialId, timestamp)
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
    serializedData.push_back( _center.x() );
    serializedData.push_back( _center.y() );
    serializedData.push_back( _center.z() );
    serializedData.push_back( _up.x() );
    serializedData.push_back( _up.y() );
    serializedData.push_back( _up.z() );
    serializedData.push_back( _centerRadius );
    serializedData.push_back( _upRadius );
    serializedData.push_back( _timestamp );
    serializedData.push_back( _value );
    return getSerializationSize();
}

size_t Cone::getSerializationSize()
{
    return 10;
}

}
