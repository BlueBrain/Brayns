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

#include "ProximityDetectionRenderer.h"

// ispc exports
#include "ProximityDetectionRenderer_ispc.h"

namespace circuitExplorer
{
void ProximityDetectionRenderer::commit()
{
    CircuitExplorerAbstractRenderer::commit();

    _nearColor = getParam3f("detectionNearColor", ospray::vec3f(0.f, 1.f, 0.f));
    _farColor = getParam3f("detectionFarColor", ospray::vec3f(1.f, 0.f, 0.f));
    _detectionDistance = getParam1f("detectionDistance", 1.f);
    _detectionOnDifferentMaterial =
        bool(getParam1i("detectionOnDifferentMaterial", 1));
    _surfaceShadingEnabled = bool(getParam1i("surfaceShadingEnabled", 1));
    _randomNumber = getParam1i("randomNumber", 0);
    _alphaCorrection = getParam1f("alphaCorrection", 0.5f);

    ispc::ProximityDetectionRenderer_set(
        getIE(), (_bgMaterial ? _bgMaterial->getIE() : nullptr),
        (ispc::vec3f&)_nearColor, (ispc::vec3f&)_farColor, _detectionDistance,
        _detectionOnDifferentMaterial, _randomNumber, _timestamp, spp,
        _surfaceShadingEnabled, _lightPtr, _lightArray.size(), _alphaCorrection,
        _maxBounces, _exposure, _useHardwareRandomizer);
}

ProximityDetectionRenderer::ProximityDetectionRenderer()
{
    ispcEquivalent = ispc::ProximityDetectionRenderer_create(this);
}

OSP_REGISTER_RENDERER(ProximityDetectionRenderer,
                      circuit_explorer_proximity_detection);
} // namespace circuitExplorer
