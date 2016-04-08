/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#ifndef TRIANGLESMESH_H
#define TRIANGLESMESH_H

#include "Geometry.h"

#include <brayns/api.h>
#include <brayns/common/types.h>
#include <vector>
#include <map>

namespace brayns
{

BRAYNS_API class TrianglesMesh : Geometry
{
public:
    BRAYNS_API TrianglesMesh();

    BRAYNS_API Vector4fs& getVertices() { return _vertices; }
    BRAYNS_API Vector4fs& getNormals() { return _normals; }
    BRAYNS_API Vector4fs& getColors() { return _colors; }
    BRAYNS_API Vector3is& getIndices() { return _indices; }
    BRAYNS_API Vector2fs& getTextureCoordinates() { return _textureCoordinates; }

private:
    Vector4fs _vertices;
    Vector4fs _normals;
    Vector4fs _colors;
    Vector3is _indices;
    Vector2fs _textureCoordinates;
};

}

#endif // TRIANGLESMESH_H
