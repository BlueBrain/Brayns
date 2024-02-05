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

#include "CircuitExplorerAdvancedRenderer.h"

#include <brayns/common/Log.h>

// ospray
#include <ospray/SDK/common/Data.h>
#include <ospray/SDK/common/Model.h>

// ispc exports
#include "CircuitExplorerAdvancedRenderer_ispc.h"

using namespace ospray;

namespace circuitExplorer
{
void CircuitExplorerAdvancedRenderer::commit()
{
    CircuitExplorerSimulationRenderer::commit();

    _shadows = getParam1f("shadows", 0.f);
    _softShadows = getParam1f("softShadows", 0.f);
    _softShadowsSamples = getParam1i("softShadowsSamples", 1);

    _giStrength = getParam1f("giWeight", 0.f);
    _giDistance = getParam1f("giDistance", 1e20f);
    _giSamples = getParam1i("giSamples", 1);
    _epsilonFactor = getParam1f("epsilonFactor", 1.f);

    _randomNumber = getParam1i("randomNumber", 0);
    _samplingThreshold = getParam1f("samplingThreshold", 0.001f);
    _volumeSamplesPerRay = getParam1i("volumeSamplesPerRay", 32);
    _volumeSpecularExponent = getParam1f("volumeSpecularExponent", 20.f);
    _volumeAlphaCorrection = getParam1f("volumeAlphaCorrection", 0.5f);

    const uint64 simulationDataSize =
        _simulationData ? _simulationData->size() : 0;

    clipPlanes = getParamData("clipPlanes", nullptr);
    const auto clipPlaneData = clipPlanes ? clipPlanes->data : nullptr;
    const uint32 numClipPlanes = clipPlanes ? clipPlanes->numItems : 0;

    ispc::CircuitExplorerAdvancedRenderer_set(
        getIE(), (_secondaryModel ? _secondaryModel->getIE() : nullptr),
        _maxDistanceToSecondaryModel,
        (_bgMaterial ? _bgMaterial->getIE() : nullptr), _shadows, _softShadows,
        _softShadowsSamples, _giStrength, _giDistance, _giSamples,
        _randomNumber, _timestamp, spp, _lightPtr, _lightArray.size(),
        _volumeSamplesPerRay,
        _simulationData ? (float*)_simulationData->data : nullptr,
        simulationDataSize, _samplingThreshold, _volumeSpecularExponent,
        _volumeAlphaCorrection, _exposure, _fogThickness, _fogStart,
        (const ispc::vec4f*)clipPlaneData, numClipPlanes, _maxBounces,
        _epsilonFactor, _useHardwareRandomizer);
}

CircuitExplorerAdvancedRenderer::CircuitExplorerAdvancedRenderer()
{
    ispcEquivalent = ispc::CircuitExplorerAdvancedRenderer_create(this);
}

OSP_REGISTER_RENDERER(CircuitExplorerAdvancedRenderer,
                      circuit_explorer_advanced);
} // namespace circuitExplorer
