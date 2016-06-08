/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#ifndef CYLINDER_H
#define CYLINDER_H

#include "Primitive.h"

#include <brayns/api.h>
#include <brayns/common/types.h>
#include <map>

namespace brayns
{

class Cylinder : public Primitive
{
public:
    BRAYNS_API Cylinder(
        size_t materialId,
        const Vector3f& center,
        const Vector3f& up,
        float radius,
        float timestamp,
        float value);

    BRAYNS_API const Vector3f& getCenter() const { return _center; }
    BRAYNS_API const Vector3f& getUp() const { return _up; }
    BRAYNS_API float getRadius() const { return _radius; }
    BRAYNS_API float getValue() const { return _value; }

    BRAYNS_API virtual size_t serializeData(floats& serializedData);
    BRAYNS_API static size_t getSerializationSize();

private:
    Vector3f _center;
    Vector3f _up;
    float _radius;
    float _value;
};

}

#endif // CYLINDER_H
