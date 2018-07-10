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
    _simulationDataSize = getParam1i("simulationDataSize", 0);
    _transferFunctionDiffuseData = getParamData("transferFunctionDiffuseData");
    _transferFunctionEmissionData =
        getParamData("transferFunctionEmissionData");
    _transferFunctionSize = getParam1i("transferFunctionSize", 0);
    _randomNumber = getParam1i("randomNumber", 0);

    ispc::ParticleRenderer_set(
        getIE(), (_bgMaterial ? _bgMaterial->getIE() : nullptr), _randomNumber,
        _timestamp, spp,
        (_simulationData ? (float*)_simulationData->data : nullptr),
        _simulationDataSize,
        _transferFunctionDiffuseData
            ? (ispc::vec4f*)_transferFunctionDiffuseData->data
            : NULL,
        (_transferFunctionEmissionData
             ? (float*)_transferFunctionEmissionData->data
             : nullptr),
        _transferFunctionSize);
}

ParticleRenderer::ParticleRenderer()
{
    ispcEquivalent = ispc::ParticleRenderer_create(this);
}

OSP_REGISTER_RENDERER(ParticleRenderer, PARTICLERENDERER);
OSP_REGISTER_RENDERER(ParticleRenderer, particlerenderer);
} // ::brayns
