/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MANIPULATOR_H
#define MANIPULATOR_H

#include <brayns/common/types.h>

namespace brayns
{

class BaseWindow;

/** Base class for camera manipulators
 */
class AbstractManipulator
{

public:

    AbstractManipulator( BaseWindow& window, KeyboardHandler& keyboardHandler );
    virtual ~AbstractManipulator() {}

    /** This is the fct that gets called when the mouse moved in the
     * associated window
     */
    virtual void motion();

    /** This is the fct that gets called when any mouse button got
     * pressed or released in the associated window
     */
    virtual void button(const Vector2i &) {}

    virtual void keypress( int32 ) {}

    virtual void specialkey( int32 ) {}

    virtual void registerKeyboardShortcuts() {}

    virtual void unregisterKeyboardShortcuts() {}

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
    KeyboardHandler& _keyboardHandler;

};

}

#endif // MANIPULATOR_H
