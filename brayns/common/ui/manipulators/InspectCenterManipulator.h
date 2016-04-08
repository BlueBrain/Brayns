/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#ifndef INSPECTCENTERMANIPULATOR_H
#define INSPECTCENTERMANIPULATOR_H

#include "AbstractManipulator.h"

namespace brayns
{

/** Defines an inspect center camera manipulators
*/
class InspectCenterManipulator : public AbstractManipulator
{
public:
    InspectCenterManipulator( BaseWindow& window );

    void dragLeft( const Vector2i &to, const Vector2i &from ) final;
    void dragRight( const Vector2i &to, const Vector2i &from ) final;
    void dragMiddle( const Vector2i &to, const Vector2i &from ) final;
    void specialkey( int32 key ) final;
    void keypress( int32 key ) final;
    void button( const Vector2i &pos ) final;

protected:
    Vector3f _pivot;
};

}
#endif // INSPECTCENTERMANIPULATOR_H
