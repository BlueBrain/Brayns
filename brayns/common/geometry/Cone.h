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
