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
#include <brayns/parameters/GeometryParameters.h>

#include <servus/types.h>

#include <vector>

namespace brayns
{

/** Simulation data hold the pointers to data contained by one single frame
 * of the simulation.
 * comparmentCounts: Number of compartments per section
 * comparmentOffsets: Offset for every compartments
 */
struct SimulationData
{
    const uint16_ts* compartmentCounts;
    const uint64_ts* compartmentOffsets;
};

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
        Scene& scene);

    /** Imports morphology from a circuit for the given target name
     *
     * @param uri URI of the Circuit Config file
     * @param target Target to be loaded. If empty, the target specified in the
     *        circuit configuration file is used. If such an entry does not
     *        exist, all neurons are loaded.
     * @param report Compartment report to be loaded
     * @param primitives Resulting primitives (spheres, cones, etc)
     * @param bounds Bounding box of the whole circuit
     * @return True if the circuit is successfully loaded, false if the circuit
     *         contains no cells.
     */
    bool importCircuit(
        const servus::URI& circuitConfig,
        const std::string& target,
        const std::string& report,
        const size_t nbSimulationFramesLoaded,
        Scene& scene);

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
        Scene& scene);

    /** Imports simulation data into a texture. Each frame of the simulation
     *  is a line of the texture, and simulation values are stored in columns.
     *  In the current implementation, RGB values are all equal to the
     *  normalized value of the data againt the whole simulation. In future
     *  work, R, G and B can be used to store different pieces of information
     *  that can then be interpreted by simulation renderer.
     * @param uri URI of the Circuit Config file
     * @param target Target to be loaded. If empty, the target specified in the
     *        circuit configuration file is used. If such an entry does not
     *        exist, all neurons are loaded.
     * @param report report to be loaded.
     * @return The number of loaded frames.
     */
    size_t importSimulationIntoTexture(
        const servus::URI& circuitConfig,
        const std::string& target,
        const std::string& report,
        Scene& scene );

    size_t createFakeSimulationTexture(
        const std::map< size_t, float>& morphologyOffsets,
        Scene& scene );

private:
    bool _importMorphology(
        const servus::URI& source,
        size_t morphologyIndex,
        const Matrix4f& transformation,
        const SimulationData* simulationData,
        PrimitivesMap& primitives,
        Boxf& bounds,
        const size_t simulationOffset,
        float& maxDistanceToSoma,
        const Vector3f& replicaPosition,
        const int32_t forcedTimestamp = -1);

    size_t _material(
        size_t morphologyIndex,
        size_t sectionType );

    GeometryParameters _geometryParameters;
};

}

#endif // MORPHOLOGY_LOADER_H
