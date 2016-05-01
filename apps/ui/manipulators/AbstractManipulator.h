/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#ifndef MANIPULATOR_H
#define MANIPULATOR_H

#include <ospray/common/OSPCommon.h>

namespace brayns
{

class BaseWindow;

/** Base class for camera manipulators
 */
class AbstractManipulator
{
public:
    AbstractManipulator( BaseWindow& window ) : _window( window ) {}
    virtual ~AbstractManipulator() {}

    /** This is the fct that gets called when the mouse moved in the
     * associated window
     */
    virtual void motion();

    /** This is the fct that gets called when any mouse button got
     * pressed or released in the associated window
     */
    virtual void button(const Vector2i &) {}

    virtual void keypress( int32 key );

    virtual void specialkey( int32 key );

protected:
    /** helper functions called from the default 'motion' function */
    virtual void dragLeft(
            const Vector2i&,
            const Vector2i&) = 0;

    virtual void dragRight(
            const Vector2i&,
            const Vector2i&) = 0;

    virtual void dragMiddle(
            const Vector2i&,
            const Vector2i&) = 0;

    BaseWindow& _window;
};

}

#endif // MANIPULATOR_H
