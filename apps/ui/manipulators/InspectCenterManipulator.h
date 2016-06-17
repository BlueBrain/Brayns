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
