/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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
