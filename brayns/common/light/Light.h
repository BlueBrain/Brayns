/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
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

#ifndef LIGHT_H
#define LIGHT_H

#include <brayns/api.h>
#include <brayns/common/types.h>

namespace brayns
{

/**
    Light object

    This object is an abstract class defining a light source with a color and an
    intensity. This is the base class for any light source available in the
    scene (Point, Directional, etc)
 */
class Light
{
public:
    BRAYNS_API virtual ~Light() = 0;

    /** Light source RGB color */
    BRAYNS_API void setColor( const Vector3f& color )
    {
        _color = color;
    }
    BRAYNS_API const Vector3f getColor( ) const { return _color; }

    /** Light source intensity */
    BRAYNS_API void setIntensity( const float intensity )
    {
        _intensity = intensity;
    }
    BRAYNS_API float getIntensity( ) const { return _intensity; }

protected:
    BRAYNS_API Light( const Vector3f& color, float intensity );

    Vector3f _color;
    float _intensity;
};

}
#endif // LIGHT_H
