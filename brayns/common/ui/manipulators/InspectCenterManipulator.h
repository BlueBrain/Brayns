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
    InspectCenterManipulator( BaseWindow& window );

    void dragLeft( const Vector2i &to, const Vector2i &from ) final;
    void dragRight( const Vector2i &to, const Vector2i &from ) final;
    void dragMiddle( const Vector2i &to, const Vector2i &from ) final;
    void specialkey( int32 key ) final;
    void keypress( int32 key ) final;
    void button( const Vector2i &pos ) final;

protected:
    Vector3f _pivot;
};

}
#endif // INSPECTCENTERMANIPULATOR_H
