/* Copyright (c) 2011-2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of Brayns and is based on the opengl-metaballs project
 * authored by Sierra Katow
 * https://github.com/sierrakatow/opengl-metaballs
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
 *
 */

#pragma once

#include <brayns/common/types.h>

/**
 * Generated a mesh according to given set of metaballs.
 */
class MetaballsGenerator
{
public:
    MetaballsGenerator() {}
    ~MetaballsGenerator();

    /** Generates a triangle based mesh model according to provided
     * metaballs, grid granularity and threshold
     *
     * @param metaballs metaballs used to generate the mesh
     * @param gridSize Size of the grid
     * @param threshold Points in 3D space that fall below the threshold
     *        (when run through the function) are ONE, while points above the
     *        threshold are ZERO
     * @param defaultMaterialId Default material to apply to the generated mesh
     * @param triangles Generated triangles
     */
    void generateMesh(const brayns::Vector4fs& metaballs, const size_t gridSize,
                      const float threshold, const size_t defaultMaterialId,
                      brayns::TriangleMeshMap& triangles);

private:
    struct SurfaceVertex
    {
        SurfaceVertex()
            : materialId(0)
        {
        }

        brayns::Vector3f position;
        brayns::Vector3f normal;
        brayns::Vector3f texCoords;
        size_t materialId;
    };

    struct CubeGridVertex : public SurfaceVertex
    {
        CubeGridVertex()
            : SurfaceVertex()
            , value(0)
        {
        }

        float value; // Value of the scalar field
    };

    struct CubeGridCube
    {
        CubeGridVertex* vertices[8];
    };

    typedef std::vector<CubeGridVertex> Vertices;
    typedef std::vector<CubeGridCube> Cubes;
    typedef std::vector<SurfaceVertex> SurfaceVertices;

    void _clear();

    void _buildVerticesAndCubes(const brayns::Vector4fs& metaballs,
                                const size_t gridSize,
                                const size_t defaultMaterialId,
                                const float scale = 5.f);

    void _buildTriangles(const brayns::Vector4fs& metaballs,
                         const float threshold, const size_t defaultMaterialId,
                         brayns::TriangleMeshMap& triangles);

    SurfaceVertices _edgeVertices;
    Vertices _vertices;
    Cubes _cubes;
};
