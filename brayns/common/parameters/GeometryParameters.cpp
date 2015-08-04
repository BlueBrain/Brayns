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

#include "GeometryParameters.h"
#include <brayns/common/log.h>
#include <brayns/common/exceptions.h>

namespace brayns
{

const std::string PARAM_RADIUS                   = "radius";
const std::string PARAM_COLORED                  = "colored";
const std::string PARAM_TIMED_GEOMETRY_INCREMENT = "timed-geometry-increment";
const std::string PARAM_REPLICAS                 = "replicas";

GeometryParameters::GeometryParameters()
  : radius_(1), colored_(false), timedGeometry_(false),
    timedGeometryIncrement_(0), replicas_(1)
{
    parameters_[PARAM_RADIUS] =
        {ptFloat, "Radius multiplier for spheres, cones and cylinders"};
    parameters_[PARAM_COLORED] =
        {ptBoolean, "Sets different color to every morphology"};
    parameters_[PARAM_TIMED_GEOMETRY_INCREMENT] =
        {ptInteger, "Increment between frames"};
    parameters_[PARAM_REPLICAS] =
        {ptInteger, "Number of random replicas for every morphology"};
}

void GeometryParameters::parse(int argc, const char **argv)
{
    for (int i=1;i<argc;i++)
    {
        std::string arg = argv[i];
        arg.erase(0,2);
        if (arg==PARAM_COLORED)
            colored_ = true;
        else if (arg==PARAM_RADIUS)
            radius_ = atof(argv[++i]);
        else if (arg==PARAM_TIMED_GEOMETRY_INCREMENT)
        {
            timedGeometry_ = true;
            timedGeometryIncrement_ = atoi(argv[++i]);
        }
        else if (arg == PARAM_REPLICAS)
            replicas_ = atoi(argv[++i]);
    }
}

void GeometryParameters::display() const
{
    BRAYNS_INFO << "Geometry options: " << std::endl;
    BRAYNS_INFO << "- Colored                  : " <<
                   (colored_ ? "on": "off") << std::endl;
    BRAYNS_INFO << "- Radius                   : " <<
                   radius_ << std::endl;
    BRAYNS_INFO << "- Timed geometry increment : " <<
                   timedGeometryIncrement_ << std::endl;
    BRAYNS_INFO << "- Number of replicas       : " <<
                   replicas_ << std::endl;
}

}
