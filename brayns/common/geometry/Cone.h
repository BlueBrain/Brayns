/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#ifndef CONE_H
#define CONE_H

#include "Primitive.h"

#include <brayns/api.h>
#include <brayns/common/types.h>
#include <map>

namespace brayns
{

class Cone : public Primitive
{
public:
    BRAYNS_API Cone(
        size_t materialId,
        const Vector3f& center,
        const Vector3f& up,
        float centerRadius,
        float upRadius,
        float timestamp = 0.f);

    BRAYNS_API const Vector3f& getCenter() const { return _center; }
    BRAYNS_API const Vector3f& getUp() const { return _up; }
    BRAYNS_API float getCenterRadius() const { return _centerRadius; }
    BRAYNS_API float getUpRadius() const { return _upRadius; }

    BRAYNS_API virtual size_t serializeData(floats& serializedData);

private:
    Vector3f _center;
    Vector3f _up;
    float _centerRadius;
    float _upRadius;
    float _timestamp;
};

}

#endif // CONE_H
