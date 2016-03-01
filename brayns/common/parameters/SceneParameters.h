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

#ifndef SCENEPARAMETERS_H
#define SCENEPARAMETERS_H

#include "AbstractParameters.h"

namespace brayns
{

class SceneParameters final : public AbstractParameters
{
public:
    SceneParameters();

    /** @copydoc AbstractParameters::parse */
    bool parse( int argc, const char **argv ) final;

    /** @copydoc AbstractParameters::print */
    void print( ) final;

    /**
       Defines the current timestamp for the scene. The unit is not universally
       specified and is therefore specific to the scene.
    */
    float getTimestamp( ) const { return _timestamp; }
    void setTimestamp( const float value ) { _timestamp = value; }

private:
    float _timestamp;
};

}
#endif // SCENEPARAMETERS_H
