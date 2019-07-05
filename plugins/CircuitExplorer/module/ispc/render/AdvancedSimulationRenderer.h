/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of the circuit explorer for Brayns
 * <https://github.com/favreau/Brayns-UC-CircuitExplorer>
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

#pragma once

#include "utils/SimulationRenderer.h"

namespace brayns
{
/**
 * @brief The AdvancedSimulationRenderer class is a renderer that can perform
 * global illumination (light shading, shadows, ambient occlusion, color
 * bleeding, light emission), mapping of simulation on the geometry, and
 * advanced rendering of volumes
 */
class AdvancedSimulationRenderer : public SimulationRenderer
{
public:
    AdvancedSimulationRenderer();

    /**
       Returns the class name as a string
       @return string containing the full name of the class
    */
    std::string toString() const final { return "AdvancedSimulationRenderer"; }
    void commit() final;

private:
    // Shading
    float _shadows;
    float _softShadows;
    int _softShadowsSamples;
    float _giStrength;
    float _giDistance;
    int _giSamples;
    int _randomNumber;

    // Volumes
    ospray::int32 _volumeSamplesPerRay;
    float _samplingThreshold;
    float _volumeSpecularExponent;
    float _volumeAlphaCorrection;

    // Clip planes
    ospray::Ref<ospray::Data> clipPlanes;
};
} // namespace brayns
