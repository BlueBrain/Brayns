/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include "utils/CircuitExplorerSimulationRenderer.h"

namespace circuitExplorer
{
/**
 * @brief The CircuitExplorerAdvancedRenderer class is a renderer that can
 * perform global illumination (light shading, shadows, ambient occlusion, color
 * bleeding, light emission), mapping of simulation on the geometry, and
 * advanced rendering of volumes
 */
class CircuitExplorerAdvancedRenderer : public CircuitExplorerSimulationRenderer
{
public:
    CircuitExplorerAdvancedRenderer();

    /**
       @return string containing the full name of the class
    */
    std::string toString() const final
    {
        return "CircuitExplorerAdvancedRenderer";
    }
    void commit() final;

private:
    // Shading
    float _shadows{0.f};
    float _softShadows{0.f};
    ospray::uint32 _softShadowsSamples{0};

    float _giStrength{0.f};
    float _giDistance{1e6f};
    ospray::uint32 _giSamples{0};
    float _epsilonFactor{1.f};

    ospray::uint32 _randomNumber{0};

    // Volumes
    float _samplingThreshold{1.f};
    ospray::int32 _volumeSamplesPerRay{32};
    float _volumeSpecularExponent{10.f};
    float _volumeAlphaCorrection{0.5f};

    // Clip planes
    ospray::Ref<ospray::Data> clipPlanes;
};
} // namespace circuitExplorer
