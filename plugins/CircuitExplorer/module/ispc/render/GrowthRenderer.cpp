/* Copyright (c) 2018, EPFL/Blue Brain Project
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

#include "GrowthRenderer.h"

// ospray
#include <ospray/SDK/common/Data.h>

// ispc exports
#include "GrowthRenderer_ispc.h"

using namespace ospray;

namespace circuitExplorer
{
void GrowthRenderer::commit()
{
    SimulationRenderer::commit();

    _simulationThreshold = getParam1f("simulationThreshold", 0.f);

    _giStrength = getParam1f("giWeight", 0.f);
    _giDistance = getParam1f("giDistance", 1e20f);
    _giSamples = getParam1i("giSamples", 1);
    _giSoftness = getParam1f("giSoftness", 0.f);

    _useTransferFunctionColor = getParam("tfColor", false);

    ispc::GrowthRenderer_set(
        getIE(), (_secondaryModel ? _secondaryModel->getIE() : nullptr),
        (_bgMaterial ? _bgMaterial->getIE() : nullptr), spp, _lightPtr,
        _lightArray.size(),
        (_simulationData ? (float*)_simulationData->data : nullptr),
        _simulationDataSize, _alphaCorrection, _simulationThreshold,
        _pixelAlpha, _fogThickness, _fogStart, _giStrength, _giDistance,
        _giSamples, _giSoftness, _useTransferFunctionColor);
}

GrowthRenderer::GrowthRenderer()
{
    ispcEquivalent = ispc::GrowthRenderer_create(this);
}

OSP_REGISTER_RENDERER(GrowthRenderer, growth_simulation);
} // namespace circuitExplorer
