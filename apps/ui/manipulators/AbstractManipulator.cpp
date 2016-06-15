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

#include <iostream>
#ifdef __APPLE__
#  include <unistd.h>
#endif

#ifdef __APPLE__
#  include "GLUT/glut.h"
#  include <unistd.h>
#else
#  include <GL/glut.h>
#  include <GL/freeglut_ext.h>
#endif

#include <apps/ui/BaseWindow.h>
#include "AbstractManipulator.h"

namespace brayns
{

void AbstractManipulator::keypress( const int32 key )
{
    switch( key )
    {
    case 'Q':
#ifdef __APPLE__
        exit(0);
#else
        glutLeaveMainLoop();
#endif
        break;
    }
}

void AbstractManipulator::specialkey( const int32 )
{
}

// ------------------------------------------------------------------
// base manipulator
// ------------------------------------------------------------------
void AbstractManipulator::motion()
{
    if(( _window.getCurrentButtonState() == (1 << GLUT_RIGHT_BUTTON )) ||
      (( _window.getCurrentButtonState() == ( 1 << GLUT_LEFT_BUTTON )) &&
      ( _window.getCurrentModifiers() & GLUT_ACTIVE_ALT )))
    {
        dragRight( _window.getCurrentMousePos(), _window.getLastMousePos() );
    }
    else if(( _window.getCurrentButtonState() == ( 1 << GLUT_MIDDLE_BUTTON )) ||
           (( _window.getCurrentButtonState() == ( 1 << GLUT_LEFT_BUTTON )) &&
           ( _window.getCurrentModifiers() & GLUT_ACTIVE_CTRL )))
    {
        dragMiddle( _window.getCurrentMousePos(), _window.getLastMousePos() );
    }
    else if (_window.getCurrentButtonState() == (1<<GLUT_LEFT_BUTTON))
    {
        dragLeft( _window.getCurrentMousePos(), _window.getLastMousePos() );
    }
}

}
