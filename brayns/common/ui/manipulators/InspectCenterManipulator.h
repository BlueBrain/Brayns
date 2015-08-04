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

#ifndef INSPECTCENTERMANIPULATOR_H
#define INSPECTCENTERMANIPULATOR_H

#include "AbstractManipulator.h"

namespace brayns
{

/** Defines an inspect center camera manipulators
*/
class InspectCenterManipulator : public AbstractManipulator
{
public:
    InspectCenterManipulator(BaseWindow *window);

    virtual void dragLeft(const ospray::vec2i &to,
                          const ospray::vec2i &from);

    virtual void dragRight(const ospray::vec2i &to,
                           const ospray::vec2i &from);

    virtual void dragMiddle(const ospray::vec2i &to,
                            const ospray::vec2i &from);

    virtual void specialkey(int32 key);

    virtual void keypress(int32 key);

    virtual void button(const ospray::vec2i &pos);

    void rotate(float du, float dv);

protected:
    ospray::vec3f pivot_;
};

}
#endif // INSPECTCENTERMANIPULATOR_H
