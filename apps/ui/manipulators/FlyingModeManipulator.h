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

    FlyingModeManipulator( BaseWindow& window, KeyboardHandler& keyboardHandler );

protected:

    void dragLeft( const Vector2i&, const Vector2i& ) final;
    void dragRight( const Vector2i&, const Vector2i& ) final;
    void dragMiddle( const Vector2i&,const Vector2i& ) final;
    void button( const Vector2i& ) final {}

    void registerKeyboardShortcuts() final;

    void unregisterKeyboardShortcuts() final;

    void StrafeLeft();
    void StrafeRight();
    void FlyForward();
    void FlyBackwards();

};

}
#endif // FLYINGMODEMAINPULATOR_H
