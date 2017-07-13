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

#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "Geometry.h"

#include <brayns/api.h>
#include <brayns/common/types.h>
#include <map>
#include <memory>

namespace brayns
{
class Primitive : public Geometry
{
public:
    BRAYNS_API Primitive(const float timestamp = 0.f,
                         const Vector2f& values = Vector2f(0.f, 0.f));
    BRAYNS_API virtual ~Primitive() {}
    BRAYNS_API float getTimestamp() const { return _timestamp; }
    BRAYNS_API virtual size_t serializeData(floats& serializedData) = 0;
    BRAYNS_API static size_t getSerializationSize()
    {
        return _serializationSize;
    }
    BRAYNS_API const Vector2f& getValues() const { return _values; }
    BRAYNS_API void setValues(const Vector2f& values) { _values = values; }
protected:
    static size_t _serializationSize;
    float _timestamp;
    Vector2f _values;
};
}

#endif // PRIMITIVE_H
