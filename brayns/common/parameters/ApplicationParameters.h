/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#ifndef APPLICATIONPARAMETERS_H
#define APPLICATIONPARAMETERS_H

#include "AbstractParameters.h"

#include <brayns/common/types.h>
#include <string>
#include <vector>

namespace brayns
{

typedef std::vector< std::string > Arguments;

/** Manages application parameters
 */
class ApplicationParameters : public AbstractParameters
{
public:
    ApplicationParameters( );

    /** @copydoc AbstractParameters::parse */
    po::variables_map  parse( int argc, const char **argv) final;

    /** @copydoc AbstractParameters::print */
    void print( ) final;

    /** window size */
    const Vector2ui& getWindowSize() const { return _windowSize; }

    /** camera name (e.g. Perspective, Stereo, etc. ) */
    std::string getCamera() const { return _camera; }

    /** DisplayCluster server host */
    std::string getDeflectHostname() const { return _deflectHostname; }
    /** Application stream name for DisplayCluster */
    std::string getDeflectStreamname() const { return _deflectStreamname; }

    /** Benchmarking */
    bool isBenchmarking() const { return _benchmarking; }

    /** JPeg compression quality */
    size_t getJpegCompression() const { return _jpegCompression; }

protected:
    bool _parse( const boost::program_options::variables_map& vm ) final;

    std::string _camera;
    Vector2ui _windowSize;
    std::string _deflectHostname;
    std::string _deflectStreamname;
    bool _benchmarking;
    size_t _jpegCompression;
};

}

#endif // APPLICATIONPARAMETERS_H
