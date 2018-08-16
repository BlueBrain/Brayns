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

#include "BasicSimulationRenderer.h"

// ospray
#include <ospray/SDK/common/Data.h>

// ispc exports
#include "BasicSimulationRenderer_ispc.h"

using namespace ospray;

namespace brayns
{
void BasicSimulationRenderer::commit()
{
    SimulationRenderer::commit();

    ispc::BasicSimulationRenderer_set(
        getIE(), (_bgMaterial ? _bgMaterial->getIE() : nullptr), spp,
        (_simulationData ? (float*)_simulationData->data : nullptr),
        _simulationDataSize,
        _transferFunctionDiffuseData
            ? (ispc::vec4f*)_transferFunctionDiffuseData->data
            : nullptr,
        (_transferFunctionEmissionData
             ? (ispc::vec3f*)_transferFunctionEmissionData->data
             : nullptr),
        _transferFunctionSize, _transferFunctionMinValue,
        _transferFunctionRange, _alphaCorrection);
}

BasicSimulationRenderer::BasicSimulationRenderer()
{
    ispcEquivalent = ispc::BasicSimulationRenderer_create(this);
}

OSP_REGISTER_RENDERER(BasicSimulationRenderer, basic_simulation);
} // ::brayns
