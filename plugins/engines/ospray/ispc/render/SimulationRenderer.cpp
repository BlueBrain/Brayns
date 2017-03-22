/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
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

#include <plugins/engines/ospray/ispc/render/SimulationRenderer.h>

// ospray
#include <ospray/SDK/common/Data.h>

// ispc exports
#include "SimulationRenderer_ispc.h"

using namespace ospray;

namespace brayns
{
void SimulationRenderer::commit()
{
    AbstractRenderer::commit();

    _volumeData = getParamData("volumeData");
    _volumeDimensions = getParam3i("volumeDimensions", ospray::vec3i(0));
    _volumeElementSpacing =
        getParam3f("volumeElementSpacing", ospray::vec3f(1.f));
    _volumeOffset = getParam3f("volumeOffset", ospray::vec3f(0.f));
    _volumeEpsilon = getParam1f("volumeEpsilon", 1.f);
    _simulationData = getParamData("simulationData");
    _transferFunctionDiffuseData = getParamData("transferFunctionDiffuseData");
    _transferFunctionEmissionData =
        getParamData("transferFunctionEmissionData");
    _transferFunctionSize = getParam1i("transferFunctionSize", 0);
    _transferFunctionMinValue = getParam1f("transferFunctionMinValue", 0.f);
    _transferFunctionRange = getParam1f("transferFunctionRange", 0.f);
    _threshold = getParam1f("threshold", _transferFunctionMinValue);

    ispc::SimulationRenderer_set(
        getIE(), (ispc::vec3f&)_bgColor, _shadowsEnabled, _softShadowsEnabled,
        _ambientOcclusionStrength, _shadingEnabled, _randomNumber, _timestamp,
        _spp, _electronShadingEnabled, _lightPtr, _lightArray.size(),
        _materialPtr, _materialArray.size(),
        _volumeData ? (uint8*)_volumeData->data : NULL,
        (ispc::vec3i&)_volumeDimensions, (ispc::vec3f&)_volumeElementSpacing,
        (ispc::vec3f&)_volumeOffset, _volumeEpsilon,
        _simulationData ? (float*)_simulationData->data : NULL,
        _transferFunctionDiffuseData
            ? (ispc::vec4f*)_transferFunctionDiffuseData->data
            : NULL,
        _transferFunctionEmissionData
            ? (ispc::vec3f*)_transferFunctionEmissionData->data
            : NULL,
        _transferFunctionSize, _transferFunctionMinValue,
        _transferFunctionRange, _threshold);
}

SimulationRenderer::SimulationRenderer()
{
    ispcEquivalent = ispc::SimulationRenderer_create(this);
}

OSP_REGISTER_RENDERER(SimulationRenderer, SIMULATIONRENDERER);
OSP_REGISTER_RENDERER(SimulationRenderer, simulationrenderer);
} // ::brayns
