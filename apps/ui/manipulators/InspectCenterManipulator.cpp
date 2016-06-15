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

#include <apps/ui/BaseWindow.h>
#include "InspectCenterManipulator.h"

namespace brayns
{

InspectCenterManipulator::InspectCenterManipulator(BaseWindow& window)
    : AbstractManipulator(window)
    , _pivot(_window.getWorldBounds().getCenter())
{
}

void InspectCenterManipulator::keypress( int32 key )
{
    Viewport& viewport = _window.getViewPort();
    switch(key)
    {
        case 'a':
            viewport.rotate(
                viewport.getTarget(), _window.getRotateSpeed(), 0, false );
            break;
        case 'd':
            viewport.rotate(
                viewport.getTarget(), -_window.getRotateSpeed(), 0, false );
            break;
        case 'w':
            viewport.rotate(
                viewport.getTarget(), 0, _window.getRotateSpeed(), false );
            break;
        case 's':
            viewport.rotate(
                viewport.getTarget(), 0, -_window.getRotateSpeed(), false );
            break;
    }

    AbstractManipulator::keypress(key);
}

void InspectCenterManipulator::button( const Vector2i& )
{
}

void InspectCenterManipulator::specialkey( int32 key )
{
    Viewport& viewport = _window.getViewPort();
    switch(key)
    {
        case GLUT_KEY_LEFT:
            viewport.rotate(
                viewport.getPosition(), _window.getRotateSpeed(), 0, false );
            break;
        case GLUT_KEY_RIGHT:
            viewport.rotate(
                viewport.getPosition(), -_window.getRotateSpeed(), 0, false );
            break;
        case GLUT_KEY_UP:
            viewport.rotate(
                viewport.getPosition(), 0, _window.getRotateSpeed(), false );
            break;
        case GLUT_KEY_DOWN:
            viewport.rotate(
                viewport.getPosition(), 0, -_window.getRotateSpeed(), false );
            break;
    }
    AbstractManipulator::specialkey( key );
}

void InspectCenterManipulator::dragRight(
        const Vector2i& to,
        const Vector2i& from)
{
    const float fwd = -( to.y() - from.y() ) * _window.getMotionSpeed();
    Viewport& viewport = _window.getViewPort();
    const float length = (viewport.getTarget() - viewport.getPosition()).length();
    const Vector3f dir( 0.f, 0.f, fwd );
    if( fwd < length )
        viewport.translate( dir, false );
}

void InspectCenterManipulator::dragMiddle(
        const Vector2i& to,
        const Vector2i& from)
{
    Viewport& viewport = _window.getViewPort();
    const float x = ( to.x() - from.x() ) * _window.getMotionSpeed();
    const float y = ( to.y() - from.y() ) * _window.getMotionSpeed();
    viewport.translate( Vector3f( x, y, 0.f ), true );
}

void InspectCenterManipulator::dragLeft(
        const Vector2i& to,
        const Vector2i& from)
{
    Viewport& viewport = _window.getViewPort();
    const float du = ( to.x() - from.x() ) * _window.getRotateSpeed();
    const float dv = ( to.y() - from.y() ) * _window.getRotateSpeed();
    viewport.rotate( viewport.getTarget(), du, dv, false );
}

}
