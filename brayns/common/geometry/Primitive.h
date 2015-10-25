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
    BRAYNS_API Primitive(const size_t materialId);
    BRAYNS_API virtual ~Primitive() {}

    BRAYNS_API size_t getMaterialId() const { return _materialId; }

    BRAYNS_API virtual size_t serializeData(floats& serializedData) = 0;

protected:
    size_t _materialId;
};

}

#endif // PRIMITIVE_H
