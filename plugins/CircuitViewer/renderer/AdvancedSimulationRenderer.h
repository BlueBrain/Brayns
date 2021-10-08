/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
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

#pragma once

#include "SimulationRenderer.h"
#include "brayns/common/types.h"
#include "brayns/common/utils/enumUtils.h"

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

    enum class Shading
    {
        none,
        diffuse,
        electron
    };

    /**
       Returns the class name as a string
       @return string containing the full name of the class
    */
    std::string toString() const final
    {
        return "brayns::AdvancedSimulationRenderer";
    }
    void commit() final;

private:
    float _shadows;
    float _softShadows;
    float _ambientOcclusionStrength;
    float _ambientOcclusionDistance;
    bool _shadingEnabled;
    bool _electronShadingEnabled;
    int _randomNumber;

    ospray::int32 _volumeSamplesPerRay;
    float _detectionDistance;
    float _samplingThreshold;
    float _volumeSpecularExponent;
    float _volumeAlphaCorrection;
};

template <>
inline std::vector<std::pair<std::string, AdvancedSimulationRenderer::Shading>>
    enumMap()
{
    return {{"none", AdvancedSimulationRenderer::Shading::none},
            {"diffuse", AdvancedSimulationRenderer::Shading::diffuse},
            {"electron", AdvancedSimulationRenderer::Shading::electron}};
}
} // namespace brayns
