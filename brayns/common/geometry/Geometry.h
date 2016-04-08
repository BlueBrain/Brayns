/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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
