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

#ifndef RENDERINGPARAMETERS_H
#define RENDERINGPARAMETERS_H

#include <brayns/common/types.h>

#include "AbstractParameters.h"


namespace brayns
{

class AbstractParameters;

/** Manages rendering parameters
 */
class RenderingParameters : public AbstractParameters
{
public:
    RenderingParameters( );

    /** @copydoc AbstractParameters::print */
    void print( ) final;

    /** OSPRay module */
    const std::string& getModule( ) const { return _module; }

    /** OSPRay renderer */
    const std::string& getRenderer( ) const { return _renderer; }
    void setRenderer( const std::string& renderer ) { _renderer = renderer; }

    /** OSPRay supported renderers */
    const strings& getRenderers( ) const { return _renderers; }

    /** Shadows */
    bool getShadows( ) const { return _shadows; }
    void setShadows( const bool value ) { _shadows = value; }

    /** Softs Shadows generated by randomizing light source position */
    bool getSoftShadows( ) const { return _softShadows; }
    void setSoftShadows( const bool value ) { _softShadows = value; }

    /** Ambient occlusion */
    float getAmbientOcclusionStrength( ) const
    {
        return _ambientOcclusionStrength;
    }
    void setAmbientOcclusionStrength( const float value )
    {
        _ambientOcclusionStrength = value;
    }

    /** Material type applied to the geometry
     */
    MaterialType getMaterialType( ) const { return _materialType; }
    void setMaterialType( const MaterialType value ) { _materialType = value; }

    /** Number of samples per pixel */
    size_t getSamplesPerPixel( ) const { return _spp; }
    void setSamplesPerPixel( const size_t value )
    {
        _spp = value;
    }

    /** Enables photon emission according to the radiance value of the
     * material */
    bool getLightEmittingMaterials( ) const { return _lightEmittingMaterials; }
    void setLightEmittingMaterials( const bool value )
    {
        _lightEmittingMaterials = value;
    }

    const Vector3f& getBackgroundColor( ) const { return _backgroundColor; }
    void setBackgroundColor( const Vector3f& value )
    {
        _backgroundColor = value;
    }

    /**
       Defines the maximum distance between intersection and surrounding
       geometry for touch detection rendering
    */
    float getDetectionDistance( ) const { return _detectionDistance; }
    void setDetectionDistance( const float value )
    {
        _detectionDistance = value;
    }

    /**
       Defines if touch detection applies if the material of the surrounding
       geometry is different from the one of the intersection
    */
    bool getDetectionOnDifferentMaterial( ) const
    {
        return _detectionOnDifferentMaterial;
    }
    void setDetectionOnDifferentMaterial( const bool value )
    {
        _detectionOnDifferentMaterial = value;
    }

    /**
       Near color used by touch detection renderer
    */
    const Vector3f& getDetectionNearColor( ) const
    {
        return _detectionNearColor;
    }
    void setDetectionNearColor( const Vector3f& value )
    {
        _detectionNearColor = value;
    }

    /**
       Far color used by touch detection renderer
    */
    const Vector3f& getDetectionFarColor( ) const { return _detectionFarColor; }
    void setDetectionFarColor( const Vector3f& value )
    {
        _detectionFarColor = value;
    }

    /**
        Raytracers have to deal with the finite precision of computer
        calculations. Since the origin of the reflected ray lies on the surface
        of the object, there will be an intersection point at zero distance.
        Since we do not want that, all intersection distances less than the
        epsilon value are ignored.
     */
    float getEpsilon() const
    {
        return _epsilon;
    }
    void setEpsilon( const float epsilon )
    {
        _epsilon = epsilon;
    }

    /**
       Camera type
    */
    CameraType getCameraType() const
    {
        return _cameraType;
    }

    /**
       Epsilon. All intersection distances less than the epsilon value are
       ignored by the raytracer.
    */
    void setEpsilon( const CameraType cameraType )
    {
        _cameraType = cameraType;
    }

    /**
       file name of the HDRI image used for the skybox
    */
    const std::string& getHDRI() const
    {
        return _hdri;
    }

    /**
       Sun follows the camera direction
    */
    bool getSunOnCamera() const
    {
        return _sunOnCamera;
    }

    /**
     * filename of the colormap used by the simulation renderer to map
     * simulation values to colors. File contains a list of RGB integer value
     * between 0 and 255 and separated by spaces.
     */
    const std::string& getSimulationColorMapFile() const
    {
        return _simulationColorMapFile;
    }

protected:

    bool _parse( const po::variables_map& vm ) final;

    std::string _module;
    std::string _renderer;
    strings _renderers;
    float _ambientOcclusionStrength;
    MaterialType _materialType;
    bool _lightEmittingMaterials;
    size_t _spp;
    bool _shadows;
    bool _softShadows;
    Vector3f _backgroundColor;
    float _detectionDistance;
    bool _detectionOnDifferentMaterial;
    Vector3f _detectionNearColor;
    Vector3f _detectionFarColor;
    float _epsilon;
    CameraType _cameraType;
    std::string _hdri;
    bool _sunOnCamera;
    std::string _simulationColorMapFile;
};

}
#endif // RENDERINGPARAMETERS_H
