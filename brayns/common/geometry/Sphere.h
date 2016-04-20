/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#ifndef SPHERE_H
#define SPHERE_H

#include "Primitive.h"

#include <brayns/api.h>
#include <brayns/common/types.h>
#include <map>

namespace brayns
{

class Sphere : public Primitive
{
public:
    BRAYNS_API Sphere(
        size_t materialId,
        const Vector3f& center,
        float radius,
        float timestamp,
        float value);

    BRAYNS_API const Vector3f& getCenter() const { return _center; }
    BRAYNS_API float getRadius() const { return _radius; }
    BRAYNS_API float getTimestamp() const { return _timestamp; }
    BRAYNS_API float getValue() const { return _value; }

    BRAYNS_API virtual size_t serializeData(floats& serializedData);
    BRAYNS_API static size_t getSerializationSize();

private:
    Vector3f _center;
    float _radius;
    float _timestamp;
    float _value;
};

}
#endif // SPHERE_H
