/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#ifndef FLYINGMODEMAINPULATOR_H
#define FLYINGMODEMAINPULATOR_H

#include "AbstractManipulator.h"

namespace brayns
{

/** Defines a flying mode camera manipulator, like in a flight simulator!
*/
class FlyingModeManipulator : public AbstractManipulator
{
public:

    FlyingModeManipulator( BaseWindow& window ) :
        AbstractManipulator( window ) {}

protected:

    void dragLeft( const Vector2i&, const Vector2i& ) final;
    void dragRight( const Vector2i&, const Vector2i& ) final;
    void dragMiddle( const Vector2i&,const Vector2i& ) final;
    void keypress( int32 key ) final;
    void button( const Vector2i& ) final {}
};

}
#endif // FLYINGMODEMAINPULATOR_H
