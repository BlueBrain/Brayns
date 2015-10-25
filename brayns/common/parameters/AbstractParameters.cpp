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

#include "AbstractParameters.h"

#include <brayns/common/log.h>
#include <string>
#include <iostream>

namespace brayns
{

const std::string PARAM_HELP = "--help";

void AbstractParameters::parse(int argc, const char **argv)
{
    for (int i=1;i<argc;i++)
    {
        std::string arg = argv[i];
        if (arg == PARAM_HELP)
            usage();
    }
}

void AbstractParameters::usage() const
{
    size_t maxLen(0);
    for( Parameters::const_iterator it=_parameters.begin();
         it!=_parameters.end(); ++it )
        maxLen = std::max(maxLen, (*it).first.length());

    BRAYNS_INFO << "Usage: " << std::endl;
    for( Parameters::const_iterator it=_parameters.begin();
         it!=_parameters.end(); ++it )
    {
        BRAYNS_INFO <<
            (*it).first << std::string(maxLen-(*it).first.length()+1, ' ') <<
            (*it).second << std::endl;
    }
}

}
