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

#ifndef ABSTRACTPARAMETERS_H
#define ABSTRACTPARAMETERS_H

#include <map>
#include <iostream>

namespace brayns
{

/** Command line parameter data types
 */
enum ParameterType
{
    ptUndefined,
    ptString,
    ptInteger,
    ptFloat,
    ptBoolean,
};

/** Base class defining parameters
 */
class Parameter
{
public:

    ParameterType type;
    std::string description;

    friend std::ostream& operator<<(std::ostream& os, const Parameter& param)
    {
        switch( param.type )
        {
        case ptString : os << "<string>"; break;
        case ptInteger: os << "<integer>"; break;
        case ptFloat  : os << "<float>"; break;
        case ptBoolean: os << "<boolean>"; break;
        default:
            os << "<undefined>"; break;
        }
        os << " " << param.description;
        return os;
    }

};
typedef std::map< std::string, Parameter > Parameters;

/** Base class defining command line parameters
 */
class AbstractParameters
{
public:
    virtual ~AbstractParameters() {}

    /** Parse the command line parameters and populates according class members
     *
     * @param argc number of command line parameters
     * @param argv actual command line parameters
     */
    virtual void parse(int argc, const char **argv);

    /** Displays parameters managed by the class
     */
    virtual void display() const = 0;

    /** Displays usage for parameters managed by the class
     */
    virtual void usage() const;

protected:
    Parameters parameters_;
};

}
#endif // ABSTRACTPARAMETERS_H
