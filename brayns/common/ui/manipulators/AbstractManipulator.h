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

#ifndef MANIPULATOR_H
#define MANIPULATOR_H

#include <ospray/common/OSPCommon.h>

namespace brayns
{

class BaseWindow;

/** Base class for camera manipulators
 */
class AbstractManipulator
{
public:
    AbstractManipulator( BaseWindow* window ) : window_(window) {}
    virtual ~AbstractManipulator() {}

    /** This is the fct that gets called when the mouse moved in the
     * associated window
     */
    virtual void motion();

    /** This is the fct that gets called when any mouse button got
     * pressed or released in the associated window
     */
    virtual void button(const ospray::vec2i &) {}

    virtual void keypress(int32 key);

    virtual void specialkey(int32 key);

protected:
    /** helper functions called from the default 'motion' function */
    virtual void dragLeft(
            const ospray::vec2i&,
            const ospray::vec2i&) {}

    virtual void dragRight(
            const ospray::vec2i&,
            const ospray::vec2i&) {}

    virtual void dragMiddle(
            const ospray::vec2i&,
            const ospray::vec2i&) {}

    BaseWindow *window_;
};

}

#endif // MANIPULATOR_H
