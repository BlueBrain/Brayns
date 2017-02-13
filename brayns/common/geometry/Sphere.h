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
    BRAYNS_API void setCenter( const Vector3f center ) { _center = center; }

    BRAYNS_API float getRadius() const { return _radius; }
    BRAYNS_API void setRadius( const float radius ) { _radius = radius; }

    BRAYNS_API float getValue() const { return _value; }
    BRAYNS_API void setValue( const float value ) { _value = value; }

    BRAYNS_API virtual size_t serializeData(floats& serializedData);
    BRAYNS_API static size_t getSerializationSize();

private:
    Vector3f _center;
    float _radius;
    float _value;
};

}
#endif // SPHERE_H
