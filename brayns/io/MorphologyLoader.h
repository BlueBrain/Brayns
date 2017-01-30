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

#ifndef MORPHOLOGY_LOADER_H
#define MORPHOLOGY_LOADER_H

#include <brayns/common/types.h>
#include <brayns/common/geometry/Primitive.h>
#include <brayns/parameters/GeometryParameters.h>

#include <servus/types.h>

#include <vector>

namespace brion
{
    class CompartmentReport;
}

namespace brayns
{

/** Simulation data hold the pointers to data contained by one single frame
 * of the simulation.
 * comparmentCounts: Number of compartments per section
 * comparmentOffsets: Offset for every compartments
 */
struct SimulationInformation
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
     * @param scene resulting scene
     * @return True if the morphology is successfully loaded, false otherwise
     */
    bool importMorphology(
        const servus::URI& uri,
        int morphologyIndex,
        Scene& scene);

    /** Imports morphology from a circuit for the given target name
     *
     * @param circuitConfig URI of the Circuit Config file
     * @param target Target to be loaded. If empty, the target specified in the
     *        circuit configuration file is used. If such an entry does not
     *        exist, all neurons are loaded.
     * @param report Compartment report to be loaded
     * @param scene resulting scene
     * @return True if the circuit is successfully loaded, false if the circuit
     *         contains no cells.
     */
    bool importCircuit(
        const servus::URI& circuitConfig,
        const std::string& target,
        const std::string& report,
        Scene& scene);

    /** Imports morphology from a circuit for the given target name
     *
     * @param circuitConfig URI of the Circuit Config file
     * @param target Target to be loaded. If empty, the target specified in the
     *        circuit configuration file is used. If such an entry does not
     *        exist, all neurons are loaded.
     * @param scene resulting scene
     * @return True if the circuit is successfully loaded, false if the circuit
     *         contains no cells.
     */
    bool importCircuit(
        const servus::URI& circuitConfig,
        const std::string& target,
        Scene& scene);

    /** Imports simulation data into the scene
     * @param circuitConfig URI of the Circuit Config file
     * @param target Target to be loaded. If empty, the target specified in the
     *        circuit configuration file is used. If such an entry does not
     *        exist, all neurons are loaded.
     * @param report report to be loaded.
     * @param scene to load the simulation data in.
     * @return True if simulation cache file successfully opened or created, false otherwise
     */
    bool importSimulationData(
        const servus::URI& circuitConfig,
        const std::string& target,
        const std::string& report,
        Scene& scene );

private:

    bool _importMorphology(
        const servus::URI& source,
        size_t morphologyIndex,
        const Matrix4f& transformation,
        const SimulationInformation* simulationInformation,
        PrimitivesMap& primitives,
        Boxf& bounds,
        const size_t simulationOffset,
        float& maxDistanceToSoma);

    bool _importMorphologyAsMesh(
        const servus::URI& source,
        const size_t morphologyIndex,
        const MaterialsMap& materials,
        const Matrix4f& transformation,
        TrianglesMeshMap& meshes,
        Boxf& bounds);

    size_t _material(
        size_t morphologyIndex,
        size_t sectionType );

    const GeometryParameters& _geometryParameters;
};

}

#endif // MORPHOLOGY_LOADER_H
