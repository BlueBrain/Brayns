/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "Light.h"

namespace brayns
{

/**
    Directional light

    Creates a light that shines from a specific direction not from a specific
    position. This light will behave as though it is infinitely far away and the
    rays produced from it are all parallel
 */
class DirectionalLight final : public Light
{
public:
    /**
        Constructor
        @param direction Light source direction
        @param color Light source RGB color
        @param intensity Amount of light emitted
     */
    BRAYNS_API DirectionalLight(
        const Vector3f& direction,
        const Vector3f& color,
        float intensity );

    BRAYNS_API ~DirectionalLight() {}

    /** Light source direction */
    BRAYNS_API void setDirection( const Vector3f direction )
    {
        _direction = direction;
    }
    BRAYNS_API const Vector3f& getDirection( ) const { return _direction; }

private:
    Vector3f _direction;
};

}
#endif // DIRECTIONALLIGHT_H
