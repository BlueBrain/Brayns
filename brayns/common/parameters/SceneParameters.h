/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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
    po::variables_map  parse( int argc, const char **argv ) final;

    /** @copydoc AbstractParameters::print */
    void print( ) final;

    /**
       Defines the current timestamp for the scene. The unit is not universally
       specified and is therefore specific to the scene.
    */
    float getTimestamp( ) const { return _timestamp; }
    void setTimestamp( const float value ) { _timestamp = value; }

protected:

    bool _parse( const po::variables_map& vm ) final;

    float _timestamp;
};

}
#endif // SCENEPARAMETERS_H
