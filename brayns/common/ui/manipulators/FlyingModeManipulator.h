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

#ifndef FLYINGMODEMAINPULATOR_H
#define FLYINGMODEMAINPULATOR_H

#include "AbstractManipulator.h"

namespace brayns
{

/** Defines a flying mode camera manipulator, like in a flight simulator!
*/
class FlyingModeManipulator : public AbstractManipulator
{
public:

    FlyingModeManipulator( BaseWindow& window ) :
        AbstractManipulator( window ) {}

protected:

    void dragLeft( const Vector2i&, const Vector2i& ) final;
    void dragRight( const Vector2i&, const Vector2i& ) final;
    void dragMiddle( const Vector2i&,const Vector2i& ) final;
    void keypress( int32 key ) final;
    void button( const Vector2i& ) final {}
};

}
#endif // FLYINGMODEMAINPULATOR_H
