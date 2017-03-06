/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
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

#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "Light.h"

namespace brayns
{
const Vector3f DEFAULT_SUN_DIRECTION = {0.1f, -0.2f, -0.5f};
const Vector3f DEFAULT_SUN_COLOR = {1.f, 1.f, 1.f};
const float DEFAULT_SUN_INTENSITY = 0.5f;
const float DEFAULT_SUN_CUTOFF = 1000.f;

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
    BRAYNS_API DirectionalLight(const Vector3f& direction,
                                const Vector3f& color, float intensity);

    BRAYNS_API ~DirectionalLight() {}
    /** Light source direction */
    BRAYNS_API void setDirection(const Vector3f direction)
    {
        _direction = direction;
    }
    BRAYNS_API const Vector3f& getDirection() const { return _direction; }
private:
    Vector3f _direction;
};
}
#endif // DIRECTIONALLIGHT_H
