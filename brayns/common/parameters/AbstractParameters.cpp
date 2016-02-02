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

#include "AbstractParameters.h"
#include <brayns/common/types.h>
#include <boost/program_options.hpp>
#include <ostream>

namespace brayns
{

namespace po = boost::program_options;

bool AbstractParameters::parse( int argc, const char **argv )
{
    po::parsed_options parsedOptions =
        po::command_line_parser( argc, argv ).options( _parameters ).
        allow_unregistered( ).run( );
    po::store( parsedOptions, _vm );
    po::notify(_vm);
    return true;
}

void AbstractParameters::usage( )
{
    BRAYNS_INFO << _name << " parameters:" <<
        std::endl << _parameters << std::endl;
}

void AbstractParameters::print( )
{
    BRAYNS_INFO << "-= " << _name << " parameters =-" << std::endl;
}

}

