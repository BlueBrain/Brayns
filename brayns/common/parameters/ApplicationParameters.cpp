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

#include "ApplicationParameters.h"
#include <brayns/common/exceptions.h>
#include <brayns/common/log.h>

#include <boost/lexical_cast.hpp>

namespace brayns
{

const std::string PARAM_BUFFER_WIDTH = "--buffer-width";
const std::string PARAM_BUFFER_HEIGHT = "--buffer-height";
const std::string PARAM_METABALLS = "--metaballs";
const std::string PARAM_STEREO_CAMERA = "--stereo-camera";
const std::string PARAM_MATERIAL_SCHEME = "--material-scheme";
const std::string PARAM_BENCHMARKING = "--enable-benchmark";

const size_t DEFAULT_WINDOW_WIDTH = 800;
const size_t DEFAULT_WINDOW_HEIGHT = 600;

const std::string PARAM_DEFLECT_HOSTNAME = "deflect-hostname";
const std::string DEFAULT_DEFLECT_HOSTNAME = "localhost";
const std::string DEFAULT_DEFLECT_STREAMNAME = "brayns";

const std::string PARAM_ZEQ_SCHEMA = "--zeq-schema";

const std::string DEFAULT_CAMERA = "perspective";

ApplicationParameters::ApplicationParameters(int argc, const char **argv)
    : AbstractParameters(argc, argv)
    , _camera(DEFAULT_CAMERA)
    , _windowWidth(DEFAULT_WINDOW_WIDTH)
    , _windowHeight(DEFAULT_WINDOW_HEIGHT)
    , _deflectHostname(DEFAULT_DEFLECT_HOSTNAME)
    , _deflectStreamname(DEFAULT_DEFLECT_STREAMNAME)
    , _benchmarking(false)
{
    _parameters[PARAM_BUFFER_HEIGHT] =
        {PMT_INTEGER, "Height of rendering buffer"};
    _parameters[PARAM_BUFFER_WIDTH] =
        {PMT_INTEGER, "Width of rendering buffer"};
    _parameters[PARAM_METABALLS] =
        {PMT_BOOLEAN, "Generates metaballs from morphologies"};
    _parameters[PARAM_STEREO_CAMERA] =
        {PMT_BOOLEAN, "Generates metaballs from morphologies"};
    _parameters[PARAM_MATERIAL_SCHEME] =
        {PMT_INTEGER, "Material scheme for molecular visualization "\
         "(0: Atoms, 1: Chains, 2: Residues)"};
    _parameters[PARAM_DEFLECT_HOSTNAME] =
        {PMT_STRING, "Name of host running DisplayCluster"};
    _parameters[DEFAULT_DEFLECT_STREAMNAME] =
        {PMT_STRING, "Name of DisplayCluster stream"};
    _parameters[PARAM_ZEQ_SCHEMA] =
        {PMT_STRING, "Schema name for ZeroEQ communication"};
    _parameters[PARAM_BENCHMARKING] =
        {PMT_STRING, "Activates application benchmarking"};

    // Save arguments for later use
    for (int i=0;i<argc;i++)
        _arguments.push_back(argv[i]);

    // Parse arguments and populate class members accordingly
    for (int i=1;i<argc;i++)
    {
        std::string arg = argv[i];
        if (arg == PARAM_STEREO_CAMERA)
            _camera = "stereo";
        else if (arg == PARAM_BUFFER_WIDTH)
            _windowWidth = boost::lexical_cast<size_t>(argv[++i]);
        else if (arg == PARAM_BUFFER_HEIGHT)
            _windowHeight = boost::lexical_cast<size_t>(argv[++i]);
        else if(arg==PARAM_ZEQ_SCHEMA)
            _zeqSchema = argv[++i];
        else if(arg==PARAM_DEFLECT_HOSTNAME)
            _deflectHostname = argv[++i];
        else if(arg==DEFAULT_DEFLECT_STREAMNAME)
            _deflectStreamname = argv[++i];
        else if(arg==PARAM_BENCHMARKING)
            _benchmarking = true;
    }
}

void ApplicationParameters::display() const
{
    BRAYNS_INFO << "Application options: " << std::endl;
    BRAYNS_INFO << "- Window size   : " << _windowWidth << "x" <<
                                           _windowHeight << std::endl;
    BRAYNS_INFO << "- Camera        : " << _camera << std::endl;
    BRAYNS_INFO << "- Rest schema   : " << _zeqSchema << std::endl;
    BRAYNS_INFO << "- Deflect       : " << _deflectHostname << ":" <<
                                           _deflectStreamname << std::endl;
    BRAYNS_INFO << "- Benchmarking  : " <<
                   (_benchmarking ? "On" : "Off") << std::endl;
}

}
