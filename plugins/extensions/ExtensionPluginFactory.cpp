/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#include "ExtensionPluginFactory.h"

#include <plugins/extensions/plugins/ExtensionPlugin.h>
#ifdef BRAYNS_USE_ZEROBUF
#  include <plugins/extensions/plugins/ZeroBufPlugin.h>
#endif
#ifdef BRAYNS_USE_DEFLECT
#  include <plugins/extensions/plugins/DeflectPlugin.h>
#endif

namespace brayns
{

ExtensionPluginFactory::ExtensionPluginFactory(
    ApplicationParameters& applicationParameters,
    ExtensionParameters& extensionParameters )
    : _applicationParameters( applicationParameters )
    , _extensionParameters( extensionParameters )
{
#ifdef BRAYNS_USE_ZEROBUF
    ZeroBufPluginPtr zeroBufPlugin( new ZeroBufPlugin(
        applicationParameters, _extensionParameters ));
    add( zeroBufPlugin );
#endif

#ifdef BRAYNS_USE_DEFLECT
    // Try to create Deflect plugin from env vars or application parameters,
    // silently ignore failure
    try
    {
        add( std::make_shared<DeflectPlugin>( applicationParameters,
                                              _extensionParameters ));
    }
    catch( ... ) {}
#endif
}

ExtensionPluginFactory::~ExtensionPluginFactory()
{
    clear( );
}

void ExtensionPluginFactory::remove( ExtensionPluginPtr plugin )
{
    ExtensionPlugins::iterator it =
        std::find( _plugins.begin( ), _plugins.end( ), plugin );
    if( it != _plugins.end( ))
        _plugins.erase( it );
}

void ExtensionPluginFactory::add( ExtensionPluginPtr plugin )
{
    remove( plugin );
    _plugins.push_back( plugin );
}

void ExtensionPluginFactory::clear( )
{
    _plugins.clear();
}

void ExtensionPluginFactory::execute( )
{
    for( ExtensionPluginPtr plugin: _plugins )
        plugin->run( );
}

}
