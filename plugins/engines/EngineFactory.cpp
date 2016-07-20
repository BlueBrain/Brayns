/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include "EngineFactory.h"

#ifdef BRAYNS_USE_OSPRAY
#  include <plugins/engines/ospray/OSPRayEngine.h>
#endif

namespace brayns
{

EngineFactory::EngineFactory(
    int argc,
    const char **argv,
    ParametersManagerPtr parametersManager )
    : _parametersManager( parametersManager )
{
    for( int i = 0; i < argc; ++i )
        _arguments.push_back( argv[i] );
}

EnginePtr EngineFactory::get( const std::string& name )
{
    if( _engines.find( name ) != _engines.end() )
        return _engines[name];
#ifdef BRAYNS_USE_OSPRAY
    if( name == "ospray" )
    {
        const char** argv = new const char*[_arguments.size()];
        for( size_t i = 0; i < _arguments.size(); ++i )
            argv[i] = _arguments[i].c_str();
        _engines[name] = EnginePtr(
            new OSPRayEngine( _arguments.size(), argv, _parametersManager ));
        delete [] argv;
        return _engines[name];
    }
#endif
    return 0;
}

}
