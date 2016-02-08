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

#ifndef MORPHOLOGY_LOADER_H
#define MORPHOLOGY_LOADER_H

#include <brayns/common/types.h>
#include <brayns/common/geometry/Primitive.h>
#include <brayns/common/parameters/GeometryParameters.h>

#include <vector>

namespace brayns
{

// File formats for morphologies
enum MorphologyFileFormat
{
    MFF_UNKNOWN = 0,
    MFF_H5,
    MFF_SWC
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

/** Loads morphologies from SWC and H5 files
 */
class MorphologyLoader
{
public:
    MorphologyLoader(
            const GeometryParameters& geometryParameters);

    /** Imports morphology from a given SWC file into spheres, cones and
     *  cylinders
     *
     * @param filename name of the file containing the morphology
     * @param morphologyIndex specifies an index for the morphology. This is
     *        mainly used to give a specific color to every morphology.
     * @param position translates the morphology to the given position
     * @param geometries resulting geom
etries
     */
    bool importMorphologies(
        MorphologyFileFormat fileFormat,
        const std::string& filename,
        int morphologyIndex,
        const Vector3f& position,
        PrimitivesMap& primitives,
        Boxf& bounds);

private:
    bool _importSWCFile(
        const std::string& filename,
        int morphologyIndex,
        const Vector3f& position,
        PrimitivesMap& primitives,
        Boxf& bounds);

    bool _importH5File(
        const std::string &filename,
        int morphologyIndex,
        const Vector3f& position,
        PrimitivesMap& primitives,
        Boxf& bounds);

    GeometryParameters _geometryParameters;
};

}

#endif // MORPHOLOGY_LOADER_H
