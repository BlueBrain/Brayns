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
#include <signal.h>
#ifdef __APPLE__
#  include <unistd.h>
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
        raise(SIGTERM);
    }
}

void AbstractManipulator::specialkey(const int32)
{
}

std::ostream &operator<<(std::ostream &os, const BaseWindow::ViewPort &cam)
{
    os << "// "
      << " -vp " << cam.from.x << " " << cam.from.y << " " << cam.from.z
      << " -vi " << cam.at.x << " " << cam.at.y << " " << cam.at.z
      << " -vu " << cam.up.x << " " << cam.up.y << " " << cam.up.z
      << std::endl;
    os << "<viewPort>" << std::endl;
    os << "  <from>" << cam.from.x << " " << cam.from.y << " " << cam.from.z <<
          "</from>" << std::endl;
    os << "  <at>" << cam.at.x << " " << cam.at.y << " " << cam.at.z <<
          "</at>" << std::endl;
    os << "  <up>" << cam.up.x << " " << cam.up.y << " " << cam.up.z
       << "</up>" << std::endl;
    os << "  <aspect>" << cam.aspect << "</aspect>" << std::endl;
    os << "  <frame.dx>" << cam.frame.l.vx << "</frame.dx>" << std::endl;
    os << "  <frame.dy>" << cam.frame.l.vy << "</frame.dy>" << std::endl;
    os << "  <frame.dz>" << cam.frame.l.vz << "</frame.dz>" << std::endl;
    os << "  <frame.p>" << cam.frame.p << "</frame.p>" << std::endl;
    os << "</viewPort>";
    return os;
}

// ------------------------------------------------------------------
// base manipulator
// ------------------------------------------------------------------
void AbstractManipulator::motion()
{
    if ((window_->getCurrentButtonState() == (1<<GLUT_RIGHT_BUTTON)) ||
            ((window_->getCurrentButtonState() == (1<<GLUT_LEFT_BUTTON)) &&
             (window_->getCurrentModifiers() & GLUT_ACTIVE_ALT)))
    {
        dragRight( window_->getCurrentMousePos(), window_->getLastMousePos() );
    }
    else if ((window_->getCurrentButtonState() == (1<<GLUT_MIDDLE_BUTTON)) ||
               ((window_->getCurrentButtonState() == (1<<GLUT_LEFT_BUTTON)) &&
                (window_->getCurrentModifiers() & GLUT_ACTIVE_CTRL)))
    {
        dragMiddle( window_->getCurrentMousePos(), window_->getLastMousePos() );
    }
    else if (window_->getCurrentButtonState() == (1<<GLUT_LEFT_BUTTON))
    {
        dragLeft( window_->getCurrentMousePos(), window_->getLastMousePos() );
    }
}

}
