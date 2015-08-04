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

#include <brayns/common/ui/BaseWindow.h>
#include "InspectCenterManipulator.h"

namespace brayns
{

InspectCenterManipulator::InspectCenterManipulator( BaseWindow *window )
    : AbstractManipulator(window)
    , pivot_(center(window_->getWorldBounds()))
{
}

void InspectCenterManipulator::keypress( int32 key )
{
    switch(key)
    {
    case 'a':
    {
        rotate(+10.f*window_->getRotateSpeed(),0);
    }
    return;
    case 'd':
    {
        rotate(-10.f*window_->getRotateSpeed(),0);
    }
    return;
    case 'w':
    {
        rotate(0,+10.f*window_->getRotateSpeed());
    }
    return;
    case 's':
    {
        rotate(0,-10.f*window_->getRotateSpeed());
    }
    return;
    }

    AbstractManipulator::keypress(key);
}

void InspectCenterManipulator::button( const ospray::vec2i& )
{
}

void InspectCenterManipulator::rotate( float du, float dv )
{
    BaseWindow::ViewPort &cam = window_->getViewPort();
    const ospray::vec3f pivot = window_->getViewPort().at;
    AffineSpace3fa xfm
            = AffineSpace3fa::translate(pivot)
            * AffineSpace3fa::rotate(cam.frame.l.vx,-dv)
            * AffineSpace3fa::rotate(cam.frame.l.vz,-du)
            * AffineSpace3fa::translate(-pivot);
    cam.frame = xfm * cam.frame;
    cam.from  = xfmPoint(xfm,cam.from);
    cam.at    = xfmPoint(xfm,cam.at);
    cam.snapUp();
    cam.modified = true;
}

void InspectCenterManipulator::specialkey( int32 key )
{
    switch(key)
    {
    case GLUT_KEY_LEFT:
    {
        rotate(+10.f*window_->getRotateSpeed(),0);
    } return;
    case GLUT_KEY_RIGHT:
    {
        rotate(-10.f*window_->getRotateSpeed(),0);
    } return;
    case GLUT_KEY_UP:
    {
        rotate(0,+10.f*window_->getRotateSpeed());
    } return;
    case GLUT_KEY_DOWN:
    {
        rotate(0,-10.f*window_->getRotateSpeed());
    } return;
    }
    AbstractManipulator::specialkey( key );
}

/*! INSPECT_CENTER::RightButton: move lookfrom/viewPort positoin
      forward/backward on right mouse button */
void InspectCenterManipulator::dragRight(
        const ospray::vec2i &to,
        const ospray::vec2i &from)
{
    BaseWindow::ViewPort &cam = window_->getViewPort();
    float fwd = -(to.y - from.y) * 4 * window_->getMotionSpeed();
    float oldDist = length(cam.at - cam.from);
    float newDist = oldDist - fwd;
    if (newDist < 1e-3f)
        return;
    cam.from = cam.at - newDist * cam.frame.l.vy;
    cam.frame.p = cam.from;
    cam.modified = true;
}

/*! INSPECT_CENTER::MiddleButton: move lookat/center of interest
      forward/backward on middle mouse button */
void InspectCenterManipulator::dragMiddle(
        const ospray::vec2i &to,
        const ospray::vec2i &from)
{
    BaseWindow::ViewPort &cam = window_->getViewPort();
    float du = (to.x - from.x);
    float dv = (to.y - from.y);

    AffineSpace3fa xfm = AffineSpace3fa::translate(
            window_->getMotionSpeed() * dv * cam.frame.l.vz ) *
            AffineSpace3fa::translate(
                -1.0 * window_->getMotionSpeed() * du * cam.frame.l.vx );

    cam.frame = xfm * cam.frame;
    cam.from = xfmPoint(xfm, cam.from);
    cam.at = xfmPoint(xfm, cam.at);
    cam.modified = true;
}

void InspectCenterManipulator::dragLeft(
        const ospray::vec2i &to,
        const ospray::vec2i &from)
{
    BaseWindow::ViewPort &cam = window_->getViewPort();
    float du = (to.x - from.x) * window_->getRotateSpeed();
    float dv = (to.y - from.y) * window_->getRotateSpeed();

    const ospray::vec3f pivot = cam.at;
    AffineSpace3fa xfm
            = AffineSpace3fa::translate(pivot)
            * AffineSpace3fa::rotate(cam.frame.l.vx,-dv)
            * AffineSpace3fa::rotate(cam.frame.l.vz,-du)
            * AffineSpace3fa::translate(-pivot);
    cam.frame = xfm * cam.frame;
    cam.from  = xfmPoint(xfm,cam.from);
    cam.at    = xfmPoint(xfm,cam.at);
    cam.snapUp();
    cam.modified = true;
}

}
