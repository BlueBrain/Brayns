/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
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
    bool parse( int argc, const char **argv) final;

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
    std::string _camera;
    Vector2ui _windowSize;
    std::string _deflectHostname;
    std::string _deflectStreamname;
    bool _benchmarking;
    size_t _jpegCompression;
};

}

#endif // APPLICATIONPARAMETERS_H
