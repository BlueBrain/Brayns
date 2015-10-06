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

#ifndef HBP_COMMON_TYPES_H
#define HBP_COMMON_TYPES_H

#include <ospray/ospray.h>
#include <ospray/common/OSPCommon.h>
#include <map>
#include <vector>

using namespace ospray;

// Use GCC intrinsics rather than OSPRay ones
#define _X86INTRIN_H_INCLUDED

typedef void* tjhandle;

namespace brayns
{

class ExtensionController;

// Geometry
enum GeometryType
{
    gt_undefined = 0,
    gt_sphere,
    gt_cylinder,
    gt_cone,
    gt_triangle,
    gt_streamline
};

struct Geometry
{
    GeometryType  type;
    vec3f v0;
    vec3f v1;
    float r;
    float l;
    float frame;
    int   materialId;
};
typedef std::vector<Geometry> Geometries;

// Cylinders
struct Cylinder
{
    vec3f v0;
    vec3f v1;
    float r;
    float frame;
};
typedef std::vector<Cylinder> Cylinders;
typedef std::map< size_t, Cylinders > CylindersCollection;

// Spheres
struct Sphere
{
    vec3f v;
    float r;
    float frame;
};
typedef std::vector< Sphere > Spheres;
typedef std::map< size_t, Spheres > SpheresCollection;

// Cones
struct Cone
{
    vec3f center;
    vec3f up;
    float r;
    float length;
    float frame;
};
typedef std::vector<Cone> Cones;
typedef std::map< size_t, Cones > ConesCollection;

// Triangles
struct Triangles
{
    std::vector<vec3fa> vertex;
    std::vector<vec3fa> normal;
    std::vector<vec4f>  color;
    std::vector<vec3i>  index;
    std::vector<vec2f>  texcoord;
};
typedef std::map< size_t, Triangles > TrianglesCollection;

// Streamlines
struct StreamLines
{
  std::vector<vec3fa> vertex;
  std::vector<int>    index;
  float radius;
  StreamLines() : radius(0.001f) {};
};
typedef std::map< size_t, StreamLines > StreamLinesCollection;

// Materials
typedef std::map< size_t, OSPMaterial > MaterialsCollection;

// File formats for morphologies
enum FileFormat
{
    ff_unknown = 0,
    ff_h5,
    ff_swc
};

struct Branch
{
    std::vector< size_t > segments;
};
typedef std::vector< Branch > Branches;

struct Morphology
{
    int   id;
    int   branch;
    float x;
    float y;
    float z;
    float radius;
    int   parent;
    int   frame;
    bool  used;
    std::vector< size_t > children;
};
typedef std::map< size_t, Morphology > Morphologies;

#if USE_TEXTURES
struct Texture2D : public RefCount {
    Texture2D()
      : channels(0)
      , depth(0)
      , width(0)
      , height(0)
      , data(NULL)
    {}

    int channels; //Number of color channels per pixel
    int depth;    //Bytes per color channel
    int width;    //Pixels per row
    int height;   //Pixels per column
    void *data;   //Pointer to binary texture data
};
#endif

// OSPRay Specific collections
typedef std::map<int, OSPGeometry> OSPGeometryCollections;
typedef std::map<int, OSPData> OSPDataCollections;

}

#endif // HBP_COMMON_TYPES_H
