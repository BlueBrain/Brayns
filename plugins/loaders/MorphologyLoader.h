/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#ifndef MORPHOLOGY_LOADER_H
#define MORPHOLOGY_LOADER_H

#include <brayns/common/types.h>
#include <brayns/common/geometry/Primitive.h>
#include <brayns/common/parameters/GeometryParameters.h>

#include <servus/types.h>

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

    /** Imports morphology from a given SWC or H5 file
     *
     * @param uri URI of the morphology
     * @param morphologyIndex specifies an index for the morphology. This is
     *        mainly used to give a specific color to every morphology.
     * @param primitives Resulting primitives (spheres, cones, etc)
     * @param bounds Bounding box of the morphology
     * @return True if the morphology is successfully loaded, false otherwise
     */
    bool importMorphology(
        const servus::URI& uri,
        int morphologyIndex,
        PrimitivesMap& primitives,
        Boxf& bounds);

    /** Imports morphology from a circuit for the given target name
     *
     * @param uri URI of the Circuit Config file
     * @param target Target to be loaded. If empty, the target specified in the
     *        circuit configuration file is used. If such an entry does not
     *        exist, all neurons are loaded.
     * @param primitives Resulting primitives (spheres, cones, etc)
     * @param bounds Bounding box of the whole circuit
     * @return True if the circuit is successfully loaded, false if the circuit
     *         contains no cells.
     */
    bool importCircuit(
        const servus::URI& circuitConfig,
        const std::string& target,
        PrimitivesMap& primitives,
        Boxf& bounds);

private:
    bool _importMorphology(
        const servus::URI& source,
        size_t morphologyIndex,
        const Matrix4f& transformation,
        PrimitivesMap& primitives,
        Boxf& bounds);

    size_t _material(
        size_t morphologyIndex,
        size_t sectionType );

    GeometryParameters _geometryParameters;
};

}

#endif // MORPHOLOGY_LOADER_H
