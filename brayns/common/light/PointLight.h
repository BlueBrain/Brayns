/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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
