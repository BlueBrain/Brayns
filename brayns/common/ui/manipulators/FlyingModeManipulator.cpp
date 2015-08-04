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

#include <ospray/common/OSPCommon.h>
#include <brayns/common/ui/BaseWindow.h>

#include "FlyingModeManipulator.h"

namespace brayns
{

void FlyingModeManipulator::keypress( int32 key )
{
    BaseWindow::ViewPort &cam = window_->getViewPort();
    switch(key) {
    case 'w':
    {
        float fwd = 1 * window_->getMotionSpeed();
        cam.from = cam.from + fwd * cam.frame.l.vy;
        cam.at   = cam.at   + fwd * cam.frame.l.vy;
        cam.frame.p = cam.from;
        cam.modified = true;
    } return;
    case 's':
    {
        float fwd = 1 * window_->getMotionSpeed();
        cam.from = cam.from - fwd * cam.frame.l.vy;
        cam.at   = cam.at   - fwd * cam.frame.l.vy;
        cam.frame.p = cam.from;
        cam.modified = true;
    } return;
    case 'd':
    {
        float fwd = 1 * window_->getMotionSpeed();
        cam.from = cam.from + fwd * cam.frame.l.vx;
        cam.at   = cam.at   + fwd * cam.frame.l.vx;
        cam.frame.p = cam.from;
        cam.modified = true;
    } return;
    case 'a':
    {
        float fwd = 1 * window_->getMotionSpeed();
        cam.from = cam.from - fwd * cam.frame.l.vx;
        cam.at   = cam.at   - fwd * cam.frame.l.vx;
        cam.frame.p = cam.from;
        cam.modified = true;
    } return;
    }
    AbstractManipulator::keypress( key );
}

void FlyingModeManipulator::dragRight(
        const ospray::vec2i &to,
        const ospray::vec2i &from)
{
    BaseWindow::ViewPort &cam = window_->getViewPort();
    float fwd =- (to.y - from.y) * 4 * window_->getMotionSpeed();
    cam.from = cam.from + fwd * cam.frame.l.vy;
    cam.at   = cam.at   + fwd * cam.frame.l.vy;
    cam.frame.p = cam.from;
    cam.modified = true;
}

void FlyingModeManipulator::dragMiddle(
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

void FlyingModeManipulator::dragLeft(
        const ospray::vec2i &to,
        const ospray::vec2i &from)
{
    BaseWindow::ViewPort &cam = window_->getViewPort();
    float du = (to.x - from.x) * window_->getRotateSpeed();
    float dv = (to.y - from.y) * window_->getRotateSpeed();

    const ospray::vec3f pivot = cam.from;
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
