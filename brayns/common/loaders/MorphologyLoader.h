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
#include <brayns/common/parameters/GeometryParameters.h>

#include <vector>

namespace brayns
{

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
     * @param bounds resulting bounding box
     */
    void importSWCMorphologies(
        const std::string& filename,
        int morphologyIndex,
        const vec3f& position,
        Geometries& geometries,
        box3f& bounds);

    /** Imports morphology from a given SWC file into streamlines
     *
     * @param filename name of the file containing the morphology
     * @param morphologyIndex specifies an index for the morphology. This is
     *        mainly used to give a specific color to every morphology.
     * @param precision specifies the precision of the streamline by considering
     *        only points at given steps of the streamline. For instance, a
     *        precision of 10 will only consider one point in 10. This is mainly
     *        used for reducing the overall size of the geometry when a large
     *        number of points are provided
     * @param streamlines resulting streamlines
     * @param bounds resulting bounding box
     */
    void importMorphologiesToStreamLines(
        const std::string& filename,
        int morphologyIndex,
        int precision,
        StreamLinesCollection streamlines,
        box3f& bounds);

#ifdef BRAYNS_USE_BBPSDK
    /** Using BBPSDK, Imports morphology from a given H5 file into spheres,
     *  cones and cylinders
     *
     * @param filename name of the file containing the morphology
     * @param morphologyIndex specifies an index for the morphology. This is
     *        mainly used to give a specific color to every morphology.
     * @param position translates the morphology to the given position
     * @param geometries resulting geometries
     * @param bounds resulting bounding box
     */
    void importH5Morphologies(
        const std::string& filename,
        int morphologyIndex,
        const vec3f& position,
        Geometries& geometries,
        box3f& bounds);
#endif

private:
    GeometryParameters geometryParameters_;
};

}

#endif // MORPHOLOGY_LOADER_H
