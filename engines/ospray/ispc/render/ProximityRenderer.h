/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * Based on OSPRay implementation
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

#ifndef PROXIMITYRENDERER_H
#define PROXIMITYRENDERER_H

#include "utils/AbstractRenderer.h"

namespace brayns
{
/**
    The ProximityRenderer uses an algorithm similar to ambient occlusion to
    identify touches between geometries. A color gradient, defined by nearColor
    and farColor, is computed according to the distance between the intersection
    that is being rendered and the surrounding geometry. nearColor is used when
    the distance to the surrounding geometry is less than 20% of the detection
    distance. farColor is used otherwise. The dection distance defines the
    maximum distance between the intersection and the surrounding geometry.

    Surrounding geometry is detected by sending random rays from the
    intersection point of the surface.

    This renderer can be configured using the following entries:
    - detectionDistance: Maximum distance for surrounding geometry detection
    - materialTestEnabled: If true, detection will be disabled for geometry that
    has the same material as the hit surface.
    - spp: Unsigned integer defining the number of samples per pixel
*/
class ProximityRenderer : public AbstractRenderer
{
public:
    ProximityRenderer();

    /**
       Returns the class name as a string
       @return string containing the full name of the class
    */
    std::string toString() const final { return "brayns::ProximityRenderer"; }
    /**
       Commits the changes held by the object so that
       attributes become available to the OSPRay rendering engine
    */
    virtual void commit();

private:
    ospray::vec3f _nearColor;
    ospray::vec3f _farColor;
    float _detectionDistance;
    bool _detectionOnDifferentMaterial;
    bool _surfaceShadingEnabled;
    int _randomNumber;
    float _alphaCorrection;
};
} // namespace brayns

#endif
