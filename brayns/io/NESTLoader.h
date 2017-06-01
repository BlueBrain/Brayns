/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Jafet Villafranca Diaz <jafet.villafrancadiaz@epfl.ch>
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

#ifndef NEST_LOADER_H
#define NEST_LOADER_H

#include <brayns/common/geometry/Primitive.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/common/types.h>
#include <brayns/parameters/GeometryParameters.h>

#include <brayns/common/simulation/AbstractSimulationHandler.h>

namespace brayns
{
/** Loads a NEST circuit from file and stores it into a cache file containing
 * the activation
 * timestamp for every GUID of every frame. The cache file full path is
 * specified by the
 * --nest-cache-file command line parameter. If the cache file does not exist,
 * it is created and
 * populated by the import process. The format of the cache file is a array of
 * float values
 * representing the activation timestamp by GUID. GUID are ordered in the same
 * way as they are
 * read from the original NEST circuit. All frames of the simulation are stored
 * one after the other.
 * The cache file contains a header of two uint64_t. The first one is the number
 * of frame, and the
 * second one is the frame size (the number of floats per frame). The cache file
 * is handled bu the
 * SpikeSimulationHandler class.
 * Note that in the current implementation, the simulation can only be played
 * forward.
 * @todo Move this loaded to Brion
 */
class NESTLoader
{
public:
    NESTLoader(const GeometryParameters& geometryParameters);

    /**
     * Imports a circuit into a scene. Every neuron is represented as a sphere,
     * with a given
     * color, and a radius of 1. The radius can be altered by the
     * --radius-multiplier command line
     * argument.
     * @param filename File containing the circuit
     * @param scene Scene in which spheres should be added
     */
    void importCircuit(const std::string& filename, Scene& scene);

    /**
     * Imports a spike report into the memory mapped cache file that will be
     * attached to the
     * specified scene at the end of the loading. If the cache file does not
     * exists, it is created.
     * The cache file contains the timestamp for the spike activation, for every
     * neuron, and for
     * every frame.
     * @param filename File containing the report
     * @return True if report was successfully imported, false otherwise
     */
    bool importSpikeReport(const std::string& filename);

private:
    bool _loadBinarySpikes(const std::string& spikesFilename);
    bool _load(const float timestamp);

    const GeometryParameters& _geometryParameters;
    floats _values;
    uint32_ts _gids;
    uint64_t _frameSize;
    uint32_t _nbElements;
    floats _spikingTimes;
    float _spikesStart;
    float _spikesEnd;

    Vector3fs _positions;
};
}

#endif // NEST_LOADER_H
