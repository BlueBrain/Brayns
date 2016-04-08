/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#ifndef EXTENSIONPLUGINFACTORY_H
#define EXTENSIONPLUGINFACTORY_H

#include <brayns/common/types.h>

namespace brayns
{

/** Manages plug-ins such as Deflect Streamer or HTTP server
 */
class ExtensionPluginFactory
{
public:
    /**
        Constructs the object and initializes default plugins according to
        application parameters.
        @param applicationParameters Application parameters
        @param extensionParameters Struture of pointers to objects that are
               potentially updated by registered plugins
    */
    ExtensionPluginFactory(
        ApplicationParameters& applicationParameters,
        ExtensionParameters& extensionParameters );

    ~ExtensionPluginFactory();

    /**
       Registers a new plug-in
       @param plugin Plug-in to register
     */
    void add( ExtensionPluginPtr plugin );

    /**
       Unregisters an existing plug-in
       @param plugin Plug-in to unregister
     */
    void remove( ExtensionPluginPtr plugin );

    /**
       Unregisters all registered plug-ins
     */
    void clear( );

    /**
       Executes code specific to every registered plugin
     */
    void execute( );

private:
    ExtensionPlugins _plugins;
    ApplicationParameters& _applicationParameters;
    ExtensionParameters& _extensionParameters;
};

}
#endif // EXTENSIONPLUGINFACTORY_H
