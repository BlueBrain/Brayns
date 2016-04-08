/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#ifndef PROXIMITYRENDERER_H
#define PROXIMITYRENDERER_H

#include <ospray/render/Renderer.h>

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
    - detectionDistance: Maximum distance for surrouding geometry detection
    - materialTestEnabled: If true, detection will be disabled for geometry that
    has the same material as the hit surface.
    - electronShadingEnabled: If true, geometries are renderered using the
    electron shading algorithm.
    - spp: Unsigned integer defining the number of samples per pixel
*/
struct ProximityRenderer : public ospray::Renderer
{
public:
    ProximityRenderer( );

    /**
       Returns the class name as a string
       @return string containing the full name of the class
    */
    std::string toString( ) const final
    {
        return "ospray::ProximityRenderer";
    }

    /**
       Commits the changes held by the object so that
       attributes become available to the OSPRay rendering engine
    */
    virtual void commit( );

private:
    std::vector< void* > lightArray;

    ospray::Data* lightData;

    float detectionDistance;
    bool detectionOnDifferentMaterial;
    bool electronShadingEnabled;
    int randomNumber;
    float timestamp;
    int spp;
};

}

#endif
