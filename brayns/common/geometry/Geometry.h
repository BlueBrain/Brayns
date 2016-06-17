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

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <brayns/api.h>

#include <cstddef>
#include <vector>

namespace brayns
{

enum GeometryType
{
    GT_UNDEFINED = 0,
    GT_SPHERE,
    GT_CYLINDER,
    GT_CONE,
    GT_TRIANGLES_MESH
};

class Geometry
{
public:
    BRAYNS_API Geometry();
    BRAYNS_API virtual ~Geometry() {}

    BRAYNS_API GeometryType getGeometryType() const { return _geometryType; }

protected:
    GeometryType _geometryType;
};

}
#endif // GEOMETRY_H
