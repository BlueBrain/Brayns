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

#ifndef APPLICATIONPARAMETERS_H
#define APPLICATIONPARAMETERS_H

#include "AbstractParameters.h"

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
    ApplicationParameters(int argc, const char **argv);

    void display() const final;

    /** window width */
    size_t getWindowWidth() const { return _windowWidth; }

    /** window height */
    size_t getWindowHeight() const { return _windowHeight; }

    /** camera name (e.g. Perspective, Stereo, etc. ) */
    std::string getCamera() const { return _camera; }

    /** ZeroEQ schema name */
    std::string getZeqSchema() const { return _zeqSchema; }

    /** Deflect parameters */
    /** DisplayCluster server host */
    std::string getDeflectHostname() const { return _deflectHostname; }
    /** Application stream name for DisplayCluster */
    std::string getDeflectStreamname() const { return _deflectStreamname; }

    /** Benchmarking */
    bool isBenchmarking() const { return _benchmarking; }

    const Arguments& getArguments() const { return _arguments; }

protected:

    std::string _camera;

    size_t _windowWidth;
    size_t _windowHeight;

    std::string _zeqSchema;

    std::string _deflectHostname;
    std::string _deflectStreamname;

    bool _benchmarking;

    Arguments _arguments;
};

}

#endif // APPLICATIONPARAMETERS_H
