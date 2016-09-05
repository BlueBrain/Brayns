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


#include <brayns/common/types.h>
#include <brayns/common/geometry/Primitive.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/parameters/GeometryParameters.h>

#include <brayns/io/simulation/SimulationHandler.h>

#include <lunchbox/memoryMap.h>
#include <servus/types.h>
#include <vector>

namespace brayns
{

/** Load a NEST circuit from file
 */
class NESTLoader : SimulationHandler
{
public:
    /**
     *
     * @param geometryParameters
     */
    NESTLoader( const GeometryParameters& geometryParameters );

    /**
     *
     * @param filepath
     * @param scene
     */
    void importCircuit( const std::string& filepath, Scene& scene );

    /**
     *
     * @param reportFile
     * @param scene
     */
    bool loadSpikeReport( const std::string& reportFile, Scene& scene );

    /**
     *
     * @return
     */
    uint64_t getFrameSize() const final { return _frameSize; }

    /**
     *
     * @param frame
     * @return
     */
    void* getFrameData( uint64_t frame ) final;

private:
    bool _loadBinarySpikes( const std::string& spikes );
    bool _load( uint64_t frame );

    const GeometryParameters& _geometryParameters;
    const float _timestep;
    std::unique_ptr< lunchbox::MemoryMap > _spikesFile;
    std::vector< float > _spikingTimes;
    float _spikesStart;
    float _spikesEnd;
    const uint32_t _nestOffset;
};

}

#endif // NEST_LOADER_H
