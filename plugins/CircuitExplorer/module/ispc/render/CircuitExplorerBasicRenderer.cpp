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

#include "CircuitExplorerBasicRenderer.h"

// ospray
#include <ospray/SDK/common/Data.h>

// ispc exports
#include "CircuitExplorerBasicRenderer_ispc.h"

using namespace ospray;

namespace circuitExplorer
{
void CircuitExplorerBasicRenderer::commit()
{
    CircuitExplorerSimulationRenderer::commit();

    _simulationThreshold = getParam1f("simulationThreshold", 0.f);

    ispc::CircuitExplorerBasicRenderer_set(
        getIE(), (_secondaryModel ? _secondaryModel->getIE() : nullptr),
        _maxDistanceToSecondaryModel,
        (_bgMaterial ? _bgMaterial->getIE() : nullptr), spp,
        (_simulationData ? (float*)_simulationData->data : nullptr),
        _simulationDataSize, _alphaCorrection, _simulationThreshold, _exposure,
        _fogThickness, _fogStart, _maxBounces, _useHardwareRandomizer);
}

CircuitExplorerBasicRenderer::CircuitExplorerBasicRenderer()
{
    ispcEquivalent = ispc::CircuitExplorerBasicRenderer_create(this);
}

OSP_REGISTER_RENDERER(CircuitExplorerBasicRenderer, circuit_explorer_basic);
} // namespace circuitExplorer
