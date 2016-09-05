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

#include "NESTLoader.h"

#include <brayns/common/log.h>
#include <brayns/common/geometry/Sphere.h>

#include <H5Cpp.h>
#include <algorithm>
#include <fstream>

namespace brayns
{

NESTLoader::NESTLoader( const GeometryParameters& geometryParameters )
    : _geometryParameters( geometryParameters )
    , _timestep( 0.1 )
    , _spikesStart( 0.f )
    , _spikesEnd( 0.f )
    , _nestOffset( 2 )
{
}

void NESTLoader::importCircuit( const std::string& filepath, Scene& scene )
{
    BRAYNS_INFO << "Loading NEST cells from circuit " << filepath << std::endl;

    H5::H5File neurons( filepath.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT );

    H5::DataSet posDataset;
    posDataset = neurons.openDataSet( "/x" );

    hsize_t dims[2];
    H5::DataSpace dspace = posDataset.getSpace();
    dspace.getSimpleExtentDims( dims );

    _frameSize = dims[0];

    std::vector< float > xPos( _frameSize );
    std::vector< float > yPos( _frameSize );
    std::vector< float > zPos( _frameSize );

    posDataset.read( xPos.data(), H5::PredType::NATIVE_FLOAT );
    posDataset = neurons.openDataSet( "/y" );
    posDataset.read( yPos.data(), H5::PredType::NATIVE_FLOAT );
    posDataset = neurons.openDataSet( "/z" );
    posDataset.read( zPos.data(), H5::PredType::NATIVE_FLOAT );

    PrimitivesMap& primitives = scene.getPrimitives();
    Boxf& bounds = scene.getWorldBounds();
    const size_t material = 0;
    const float radius = 5.f * _geometryParameters.getRadiusMultiplier();

    for( size_t gid = 0; gid < _frameSize; ++gid )
    {
        const Vector3f center( xPos[gid], yPos[gid], zPos[gid] );
        primitives[material].push_back( SpherePtr(
            new Sphere( material, center, radius,
                        /*timestamp*/ 0.f, /* value */ gid )));
        bounds.merge( center );
    }
    BRAYNS_INFO << "Finished loading " << _frameSize << " neurons" << std::endl;
}

bool NESTLoader::_loadBinarySpikes( const std::string& spikes )
{
    std::unique_ptr< lunchbox::MemoryMap > spikesFile
            ( new lunchbox::MemoryMap( spikes ));
    const size_t size = spikesFile->getSize();
    if(( size % sizeof( uint32_t )) != 0 )
        return false;
    const size_t nElems = size / sizeof( uint32_t );
    const uint32_t* iData = spikesFile->getAddress< uint32_t >();
    size_t index = 0;

    const uint32_t magic = 0xf0a;
    const uint32_t version = 1;
    if( index >= nElems || iData[ index++ ] != magic )
        return false;
    if( index >= nElems || iData[ index++ ] != version )
        return false;

    _spikesFile = std::move( spikesFile );
    const float* fData = _spikesFile->getAddress< float >();
    _spikesStart = fData[index]; // first spike timestamp after header
    _spikesEnd = fData[nElems - 2]; // last spike timestamp
    return true;
}

bool NESTLoader::loadSpikeReport( const std::string& reportFile, Scene& scene )
{
    if( !_loadBinarySpikes( reportFile ))
    {
        BRAYNS_ERROR << "No valid binary .spikes file found" << std::endl;
        return false;
    }

    const uint64_t nbFrames = ( _spikesEnd - _spikesStart ) / _timestep;
    _spikingTimes.resize( _frameSize, -1.f );
    scene.setSimulationHandler( this );

    BRAYNS_INFO << "----------------------------------------" << std::endl;
    BRAYNS_INFO << "Number of frames: " << nbFrames << std::endl;
    BRAYNS_INFO << "Frame size      : " << _frameSize << std::endl;
    BRAYNS_INFO << "----------------------------------------" << std::endl;
    return true;
}

bool NESTLoader::_load( uint64_t frame )
{
    if( _currentFrame == frame )
        return true;

//    _spikingTimes.assign( _spikingTimes.size(), -1.f );
    const size_t size = _spikesFile->getSize();
    const size_t nElems = size / sizeof( uint32_t );
    const uint32_t* iData = _spikesFile->getAddress< uint32_t >();
    const float* fData = _spikesFile->getAddress< float >();

    const float start = frame * _timestep;
    const float end = ( 1 + frame ) * _timestep;

    BRAYNS_INFO << "Loading spikes at frame: " << frame
                << " (" << start << " ms)" << std::endl;

    size_t numSpikes = 0;
    for( size_t i = 2; i+1 < nElems; i += 2 )
    {
        const float time = fData[ i ];
        // whith the next check, simulation only plays forward
        if( time < start )
            continue;

        if( time >= end )
            break;

        const uint32_t gid = iData[ i + 1 ] - _nestOffset;
        // we store the frame on which the spike happens, as the renderer keeps
        // track of the current frame (not timestamp)
        _spikingTimes[gid] = time / _timestep;

        ++numSpikes;
    }

    std::cout << numSpikes << std::endl;
    _currentFrame = frame;

    return true;
}

void* NESTLoader::getFrameData( const uint64_t frame )
{
    _load( frame );
    return (unsigned char*)_spikingTimes.data();
}

}
