/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#include <apps/ui/BaseWindow.h>

#include "FlyingModeManipulator.h"

namespace brayns
{

void FlyingModeManipulator::keypress( int32 key )
{
    Viewport& viewport = _window.getViewPort();
    const Vector3f dir =
        normalize( viewport.getTarget() - viewport.getPosition( ));
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
            viewport.translate(
                vmml::cross( dir, viewport.getUp( ))*fwd, true );
            break;
        }
        case 'a':
        {
            const float fwd = _window.getMotionSpeed();
            viewport.translate(
                -vmml::cross( dir, viewport.getUp( ))*fwd, true );
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
    const Vector3f dir =
        normalize( viewport.getTarget() - viewport.getPosition( ));
    viewport.translate( dir*fwd, true );
}

void FlyingModeManipulator::dragMiddle(
        const Vector2i& to,
        const Vector2i& from )
{
    Viewport& viewport = _window.getViewPort();
    const float x = ( to.x() - from.x( )) * _window.getMotionSpeed();
    const float y = ( to.y() - from.y( )) * _window.getMotionSpeed();
    const Vector3f dir =
        normalize( viewport.getTarget() - viewport.getPosition( ));
    viewport.translate( Vector3f( -y, x, 0.f ).cross( dir ), true );
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
