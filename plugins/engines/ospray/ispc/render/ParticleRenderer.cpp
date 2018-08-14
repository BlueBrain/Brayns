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

#include <brayns/common/log.h>
#include <plugins/engines/ospray/ispc/render/ParticleRenderer.h>

// ospray
#include <ospray/SDK/common/Data.h>

// ispc exports
#include "ParticleRenderer_ispc.h"

using namespace ospray;

namespace brayns
{
void ParticleRenderer::commit()
{
    AbstractRenderer::commit();

    _simulationData = getParamData("simulationData");
    _transferFunctionDiffuseData = getParamData("transferFunctionDiffuseData");
    _transferFunctionEmissionData =
        getParamData("transferFunctionEmissionData");
    _transferFunctionMinValue = getParam1f("transferFunctionMinValue", 0.f);
    _transferFunctionRange = getParam1f("transferFunctionRange", 0.f);
    _alphaCorrection = getParam1f("alphaCorrection", 0.5f);

    const auto transferFunctionDiffuseSize =
        _transferFunctionDiffuseData ? _transferFunctionDiffuseData->size() : 0;
    const auto transferFunctionEmissionSize =
        _transferFunctionEmissionData ? _transferFunctionEmissionData->size()
                                      : 0;

    if (transferFunctionDiffuseSize != transferFunctionEmissionSize)
        BRAYNS_ERROR << "Transfer function diffuse/emission size not the same: "
                     << "'" << transferFunctionDiffuseSize << "' vs '"
                     << transferFunctionEmissionSize << "'" << std::endl;

    const auto transferFunctionSize =
        std::min(transferFunctionDiffuseSize, transferFunctionEmissionSize);

    const auto simulationDataSize =
        _simulationData ? _simulationData->size() : 0;

    ispc::ParticleRenderer_set(
        getIE(), (_bgMaterial ? _bgMaterial->getIE() : nullptr), _timestamp,
        spp, (_simulationData ? (float*)_simulationData->data : nullptr),
        simulationDataSize,
        _transferFunctionDiffuseData
            ? (ispc::vec4f*)_transferFunctionDiffuseData->data
            : nullptr,
        (_transferFunctionEmissionData
             ? (ispc::vec3f*)_transferFunctionEmissionData->data
             : nullptr),
        transferFunctionSize, _transferFunctionMinValue, _transferFunctionRange,
        _alphaCorrection);
}

ParticleRenderer::ParticleRenderer()
{
    ispcEquivalent = ispc::ParticleRenderer_create(this);
}

OSP_REGISTER_RENDERER(ParticleRenderer, particle);
} // ::brayns
