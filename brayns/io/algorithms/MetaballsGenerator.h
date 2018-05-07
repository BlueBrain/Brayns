/* Copyright (c) 2011-2017, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#ifndef METABALLSGENERATOR_H
#define METABALLSGENERATOR_H

#include <brayns/common/types.h>

namespace brayns
{
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
    void generateMesh(const Vector4fs& metaballs, const size_t gridSize,
                      const float threshold, const size_t defaultMaterialId,
                      TrianglesMeshMap& triangles);

private:
    struct SurfaceVertex
    {
        SurfaceVertex()
            : materialId(0)
        {
        }

        Vector3f position;
        Vector3f normal;
        Vector3f texCoords;
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

    void _buildVerticesAndCubes(const Vector4fs& metaballs,
                                const size_t gridSize,
                                const size_t defaultMaterialId,
                                const float scale = 5.f);

    void _buildTriangles(const Vector4fs& metaballs, const float threshold,
                         const size_t defaultMaterialId,
                         TrianglesMeshMap& triangles);

    SurfaceVertices _edgeVertices;
    Vertices _vertices;
    Cubes _cubes;
};
}
#endif // METABALLSGENERATOR_H
