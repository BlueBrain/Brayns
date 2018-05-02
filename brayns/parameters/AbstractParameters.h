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

#ifndef ABSTRACTPARAMETERS_H
#define ABSTRACTPARAMETERS_H

#include <brayns/common/BaseObject.h>
#include <brayns/common/log.h>
#include <brayns/common/types.h>

namespace po = boost::program_options;

namespace brayns
{
/**
   Base class defining command line parameters
 */
class AbstractParameters : public BaseObject
{
public:
    /**
       Constructor
       @param name Display name for the set of parameters
     */
    AbstractParameters(const std::string& name)
        : _name(name)
        , _parameters(name)
    {
    }

    virtual ~AbstractParameters() = default;
    /**
       Parses parameters managed by the class
       @param vm the variables map of all arguments passed by the user
     */
    virtual void parse(const po::variables_map& vm) = 0;

    /**
       Displays values of registered parameters
     */
    virtual void print();

    po::options_description& parameters() { return _parameters; }
protected:
    std::string _name;

    po::options_description _parameters;
};
}
#endif // ABSTRACTPARAMETERS_H
