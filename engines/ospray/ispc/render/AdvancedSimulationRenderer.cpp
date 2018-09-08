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

#include "AdvancedSimulationRenderer.h"
#include <brayns/common/log.h>

// ospray
#include <ospray/SDK/common/Data.h>
#include <ospray/SDK/common/Model.h>

// ispc exports
#include "AdvancedSimulationRenderer_ispc.h"

using namespace ospray;

namespace brayns
{
void AdvancedSimulationRenderer::commit()
{
    AbstractRenderer::commit();

    _shadows = getParam1f("shadows", 0.f);
    _softShadows = getParam1f("softShadows", 0.f);
    _ambientOcclusionStrength = getParam1f("aoWeight", 0.f);
    _ambientOcclusionDistance = getParam1f("aoDistance", 1e20f);
    _detectionDistance = getParam1f("detectionDistance", 15.f);

    _randomNumber = getParam1i("randomNumber", 0);

    _simulationModel = (ospray::Model*)getParamObject("simulationModel", 0);
    _volumeSamplesPerRay = getParam1i("volumeSamplesPerRay", 32);
    _simulationData = getParamData("simulationData");
    _transferFunctionDiffuseData = getParamData("transferFunctionDiffuseData");
    _transferFunctionEmissionData =
        getParamData("transferFunctionEmissionData");
    _transferFunctionMinValue = getParam1f("transferFunctionMinValue", 0.f);
    _transferFunctionRange = getParam1f("transferFunctionRange", 0.f);

    _samplingThreshold = getParam1f("samplingThreshold", 0.001f);
    _volumeSpecularExponent = getParam1f("volumeSpecularExponent", 20.f);
    _volumeAlphaCorrection = getParam1f("volumeAlphaCorrection", 0.5f);

    const auto transferFunctionDiffuseSize =
        _transferFunctionDiffuseData ? _transferFunctionDiffuseData->size() : 0;
    const auto transferFunctionEmissionize =
        _transferFunctionEmissionData ? _transferFunctionEmissionData->size()
                                      : 0;

    if (transferFunctionDiffuseSize != transferFunctionEmissionize)
        BRAYNS_ERROR << "Transfer function diffuse/emission size not the same: "
                     << "'" << transferFunctionDiffuseSize << "' vs '"
                     << transferFunctionEmissionize << "'" << std::endl;

    const auto transferFunctionSize =
        std::min(transferFunctionDiffuseSize, transferFunctionEmissionize);

    const auto simulationDataSize =
        _simulationData ? _simulationData->size() : 0;

    ispc::AdvancedSimulationRenderer_set(
        getIE(), (_simulationModel ? _simulationModel->getIE() : nullptr),
        (_bgMaterial ? _bgMaterial->getIE() : nullptr), _shadows, _softShadows,
        _ambientOcclusionStrength, _ambientOcclusionDistance, _randomNumber,
        _timestamp, spp, _lightPtr, _lightArray.size(), _volumeSamplesPerRay,
        _simulationData ? (float*)_simulationData->data : NULL,
        simulationDataSize,
        _transferFunctionDiffuseData
            ? (ispc::vec4f*)_transferFunctionDiffuseData->data
            : NULL,
        _transferFunctionEmissionData
            ? (ispc::vec3f*)_transferFunctionEmissionData->data
            : NULL,
        transferFunctionSize, _transferFunctionMinValue, _transferFunctionRange,
        _samplingThreshold, _detectionDistance, _volumeSpecularExponent,
        _volumeAlphaCorrection);
}

AdvancedSimulationRenderer::AdvancedSimulationRenderer()
{
    ispcEquivalent = ispc::AdvancedSimulationRenderer_create(this);
}

OSP_REGISTER_RENDERER(AdvancedSimulationRenderer, advanced_simulation);
} // namespace brayns
