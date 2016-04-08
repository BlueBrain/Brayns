/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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

void AbstractManipulator::keypress( const int32 key )
{
    switch( key )
    {
    case 'Q':
        glutLeaveMainLoop();
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
