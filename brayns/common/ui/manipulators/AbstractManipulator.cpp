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

#include <brayns/common/ui/BaseWindow.h>
#include "AbstractManipulator.h"

namespace brayns
{

void AbstractManipulator::keypress(const int32 key)
{
    switch(key)
    {
    case 'Q':
        glutLeaveMainLoop();
        break;
    }
}

void AbstractManipulator::specialkey(const int32)
{
}

// ------------------------------------------------------------------
// base manipulator
// ------------------------------------------------------------------
void AbstractManipulator::motion()
{
    if ((_window.getCurrentButtonState() == (1<<GLUT_RIGHT_BUTTON)) ||
            ((_window.getCurrentButtonState() == (1<<GLUT_LEFT_BUTTON)) &&
             (_window.getCurrentModifiers() & GLUT_ACTIVE_ALT)))
    {
        dragRight( _window.getCurrentMousePos(), _window.getLastMousePos() );
    }
    else if ((_window.getCurrentButtonState() == (1<<GLUT_MIDDLE_BUTTON)) ||
               ((_window.getCurrentButtonState() == (1<<GLUT_LEFT_BUTTON)) &&
                (_window.getCurrentModifiers() & GLUT_ACTIVE_CTRL)))
    {
        dragMiddle( _window.getCurrentMousePos(), _window.getLastMousePos() );
    }
    else if (_window.getCurrentButtonState() == (1<<GLUT_LEFT_BUTTON))
    {
        dragLeft( _window.getCurrentMousePos(), _window.getLastMousePos() );
    }
}

}
