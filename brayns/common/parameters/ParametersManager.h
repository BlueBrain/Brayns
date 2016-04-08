/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#ifndef PARAMETERSMANAGER_H
#define PARAMETERSMANAGER_H

#include <brayns/api.h>
#include <brayns/common/types.h>
#include <boost/program_options.hpp>
#include <brayns/common/parameters/ApplicationParameters.h>
#include <brayns/common/parameters/RenderingParameters.h>
#include <brayns/common/parameters/GeometryParameters.h>
#include <brayns/common/parameters/SceneParameters.h>

namespace brayns
{

/**
   Class managing all parameters registered by the application. By default
   this class create an instance of Application, Rendering, and Geometry
   parameters are registered. Other parameters can also be added using the
   registerParameters method for as long as they are inherited from
   AbstractParameters.
 */
class ParametersManager
{
public:
    ParametersManager();

    /**
       Registers specific parameters to the manager
       @param parameters to be registered
     */
    BRAYNS_API void registerParameters( AbstractParameters* parameters );

    /**
       Displays help screen for registered parameters
     */
    BRAYNS_API void printHelp( );

    /**
       Displays values registered parameters
     */
    BRAYNS_API void print( );

    /**
       Parses registered parameters
       @param argc number of command line parameters
       @param argv actual command line parameters
     */
    BRAYNS_API void parse( int argc, const char **argv );

    /**
       Gets rendering parameters
       @return Rendering parameters for the current scene
    */
    BRAYNS_API RenderingParameters& getRenderingParameters();

    /**
       Gets geometry parameters
       @return Geometry parameters for the current scene
    */
    BRAYNS_API GeometryParameters& getGeometryParameters();

    /**
       Gets application parameters
       @return Application parameters for the current scene
    */
    BRAYNS_API ApplicationParameters& getApplicationParameters();

    /**
       Gets scene parameters
       @return Parameters for the current scene
    */
    BRAYNS_API SceneParameters& getSceneParameters();

private:
    std::vector< AbstractParameters* > _parameterSets;
    boost::program_options::options_description _parameters;
    ApplicationParameters _applicationParameters;
    RenderingParameters _renderingParameters;
    GeometryParameters _geometryParameters;
    SceneParameters _sceneParameters;
};

}
#endif // PARAMETERSMANAGER_H
