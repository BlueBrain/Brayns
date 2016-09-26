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

    BRAYNS_API Vector3fs& getVertices() { return _vertices; }
    BRAYNS_API Vector3fs& getNormals() { return _normals; }
    BRAYNS_API Vector4fs& getColors() { return _colors; }
    BRAYNS_API Vector3is& getIndices() { return _indices; }
    BRAYNS_API Vector2fs& getTextureCoordinates() { return _textureCoordinates; }

private:
    Vector3fs _vertices;
    Vector3fs _normals;
    Vector4fs _colors;
    Vector3is _indices;
    Vector2fs _textureCoordinates;
};

}

#endif // TRIANGLESMESH_H
