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
#include <memory>
#include <map>

namespace brayns
{

class Primitive: public Geometry
{
public:
    BRAYNS_API Primitive( const size_t materialId, const float timestamp = 0.f);
    BRAYNS_API virtual ~Primitive() {}

    BRAYNS_API size_t getMaterialId() const { return _materialId; }
    BRAYNS_API float getTimestamp() const { return _timestamp; }

    BRAYNS_API virtual size_t serializeData(floats& serializedData) = 0;
    BRAYNS_API static size_t getSerializationSize()
    { return _serializationSize; }


protected:
    static size_t _serializationSize;
    size_t _materialId;
    float _timestamp;
};

}

#endif // PRIMITIVE_H
