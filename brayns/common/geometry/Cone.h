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
        float timestamp,
        float value);

    BRAYNS_API const Vector3f& getCenter() const { return _center; }
    BRAYNS_API void setCenter( const Vector3f center ) { _center = center; }

    BRAYNS_API const Vector3f& getUp() const { return _up; }
    BRAYNS_API void setUp( const Vector3f up ) { _up = up; }

    BRAYNS_API float getCenterRadius() const { return _centerRadius; }
    BRAYNS_API void setCenterRadius( const float radius ) { _centerRadius = radius; }

    BRAYNS_API float getUpRadius() const { return _upRadius; }
    BRAYNS_API void setUpRadius( const float radius ) { _upRadius = radius; }

    BRAYNS_API virtual size_t serializeData(floats& serializedData);
    BRAYNS_API static size_t getSerializationSize();

private:
    Vector3f _center;
    Vector3f _up;
    float _centerRadius;
    float _upRadius;
    float _value;
};

}

#endif // CONE_H
