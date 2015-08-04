/* Copyright (c) 2011-2015, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns and is based on the opengl-metaballs project
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
#include <vector>
#include <map>

namespace brayns
{

struct MetaballSphere
{
    MetaballSphere() : materialId(0) {}
    brayns::Sphere sphere;
    size_t materialId;
};
typedef std::vector<MetaballSphere> MetaballSpheres;

struct CubeGridVertex
{
    CubeGridVertex() : materialId(0), value(0) {}

    vec3fa position;
    vec3fa normal;
    vec3fa textCoords;
    size_t materialId;
    float  value;	//the value of the scalar field at this point
};

struct SurfaceVertex
{
    SurfaceVertex() : materialId(0) {}

    vec3fa position;
    vec3fa normal;
    vec3fa texCoords;
    size_t materialId;
};

struct CubeGridCube
{
    CubeGridVertex* vertices[8];
};

struct CubeGrid
{
    bool CreateMemory();
    bool Init(int gridSize_);
    void DrawSurface(float threshold);
    void FreeMemory();

    CubeGrid() :
        numVertices_(0), vertices_(0),
        numCubes_(0), cubes_(0), numFacesDrawn_(0)
    {}
    ~CubeGrid() { FreeMemory();}

    int numVertices_;
    CubeGridVertex* vertices_;
    int numCubes_;
    CubeGridCube* cubes_;
    int numFacesDrawn_;
};

typedef std::map<size_t, CubeGridVertex> Vertices;
typedef std::map<size_t, CubeGridCube> Cubes;

/** Generated metaballs according to given set of spheres. The resulting
 * model is a set of triangles.
 */
struct MetaballsGenerator
{
public:
    /** Generates a triangle based metaballs model according to provided
     * spheres, grid granularity and threshold
     *
     * @param bounds bouding box defining the grid size
     * @param spheres spheres to generate the metaballs from
     * @param triangles generated triangles
     * @param gridGranularity granularity of the grid
     * @param threshold points in 3D space that fall below the threshold
     *        (when run through the function) are ONE, while points above the
     *        threshold are ZERO
     */
    MetaballsGenerator(
            const box3f& bounds,
            const MetaballSpheres& spheres,
            brayns::TrianglesCollection& triangles,
            size_t gridGranularity,
            float threshold );
};

}
#endif // METABALLSGENERATOR_H
