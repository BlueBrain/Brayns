/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
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

#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Light.h"

namespace brayns
{

/**
    Point light object

    This object is a light source defined by a position, a color, an intensity
    and a cutoff distance
 */
class PointLight final : public Light
{
public:
    /**
        Constructor
        @param position Light source position
        @param color Light source RGB color
        @param intensity Amount of light emitted
        @param cutoffDistance Cutoff distance
     */
    BRAYNS_API PointLight(
        const Vector3f& position,
        const Vector3f& color,
        float intensity,
        float cutoffDistance );

    BRAYNS_API ~PointLight() {}

    /** Light source position */
    BRAYNS_API void setPosition( const Vector3f& position )
    {
        _position = position;
    }
    BRAYNS_API const Vector3f& getPosition( ) const
    {
        return _position;
    }

    /** Light source cutoff distance */
    BRAYNS_API void setCutoffDistance( float cutoffDistance )
    {
        _cutoffDistance = cutoffDistance;
    }
    BRAYNS_API float getCutoffDistance( ) const
    {
        return _cutoffDistance;
    }

private:
    Vector3f _position;
    float _cutoffDistance;
};

}
#endif // POINTLIGHT_H
