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

#ifndef GEOMETRYPARAMETERS_H
#define GEOMETRYPARAMETERS_H

#include "AbstractParameters.h"

#include <stdio.h>

namespace brayns
{

/** Manages geometry parameters
 */
class GeometryParameters : public AbstractParameters
{
public:
    GeometryParameters();

    /** Parse the command line parameters and populates according class members
     *
     * @param argc number of command line parameters
     * @param argv actual command line parameters
     */
    void parse(int argc, const char **argv) final;

    /** Displays parameters managed by the class
     */
    void display() const final;

    /** Radius multiplier applied to spheres, cones and cylinders */
    float getRadius() const { return radius_; }
    void setRadius( float value) { radius_ = value; }

    /** Enables a different color for every morphology/mesh when
    * loading them from a given folder
    */
    bool getColored() const { return colored_; }
    void setColored( bool value) { colored_ = value; }

    /** Enables timed geometries by incrementing the timestamp for
     * every computer frame */
    bool getTimedGeometry() const { return timedGeometry_; }
    void setTimedGeometry( bool value) { timedGeometry_ = value; }

    /** Defines the timestamp increment between two frames */
    size_t getTimedGeometryIncrement() const
    { return timedGeometryIncrement_; }
    void setTimedGeometryIncrement( size_t value)
    { timedGeometryIncrement_ = value; }

    /** For testing purpose only: Multiplies the number of instances of a
     * morphology and randomly sets its position. This is used to simulate
     * large data sets as well as performance testing
     */
    size_t getReplicas() const { return replicas_; }
    void setReplicas( size_t value) { replicas_ = value; }

protected:
    float  radius_;
    bool   colored_;
    bool   timedGeometry_;
    size_t timedGeometryIncrement_;
    size_t replicas_;
};

}
#endif // GEOMETRYPARAMETERS_H
