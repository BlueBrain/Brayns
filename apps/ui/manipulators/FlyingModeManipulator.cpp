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

#include "FlyingModeManipulator.h"

#include <apps/ui/BaseWindow.h>
#include <brayns/common/input/KeyboardHandler.h>

namespace brayns
{

FlyingModeManipulator::FlyingModeManipulator(
    BaseWindow& window,
    KeyboardHandler& keyboardHandler )
    : AbstractManipulator( window, keyboardHandler )
{
    _keyboardHandler.registerKey( 'a', "Strafe left" );
    _keyboardHandler.registerKey( 'd', "Strafe right" );
    _keyboardHandler.registerKey( 'w', "Fly forward" );
    _keyboardHandler.registerKey( 's', "Fly backwards" );
}

void FlyingModeManipulator::keypress( int32 key )
{
    Viewport& viewport = _window.getViewPort();
    const Vector3f dir( 0.f, 0.f , 1.f );
    const Vector3f strafe( 1.f, 0.f , 0.f );
    switch(key) {
        case 'w':
        {
            const float fwd = _window.getMotionSpeed();
            viewport.translate( dir*fwd, true );
            break;
        }
        case 's':
        {
            const float fwd = _window.getMotionSpeed();
            viewport.translate( -dir*fwd, true );
            break;
        }
        case 'd':
        {
            const float fwd = _window.getMotionSpeed();
            viewport.translate(-strafe*fwd, true );
            break;
        }
        case 'a':
        {
            const float fwd = _window.getMotionSpeed();
            viewport.translate(strafe*fwd, true );
            break;
        }
    }
    AbstractManipulator::keypress( key );
}

void FlyingModeManipulator::dragRight(
        const Vector2i& to,
        const Vector2i& from )
{
    const float fwd = -( to.y() - from.y( )) * _window.getMotionSpeed();
    Viewport& viewport = _window.getViewPort();
    const Vector3f dir( 0.f, 0.f , 1.f );
    viewport.translate( dir*fwd, false );
}

void FlyingModeManipulator::dragMiddle(
        const Vector2i& to,
        const Vector2i& from )
{
    Viewport& viewport = _window.getViewPort();
    const float x = ( to.x() - from.x( )) * _window.getMotionSpeed();
    const float y = ( to.y() - from.y( )) * _window.getMotionSpeed();
    viewport.translate( Vector3f( x, y , 0.f ), true );
}

void FlyingModeManipulator::dragLeft(
        const Vector2i& to,
        const Vector2i& from )
{
    const float du = ( to.x() - from.x( )) * _window.getRotateSpeed();
    const float dv = ( to.y() - from.y( )) * _window.getRotateSpeed();
    Viewport& viewport = _window.getViewPort();
    viewport.rotate( viewport.getPosition(), du, dv, true );
}

}
