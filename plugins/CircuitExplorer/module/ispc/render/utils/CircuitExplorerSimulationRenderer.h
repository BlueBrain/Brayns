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

#pragma once

// obj
#include "../CircuitExplorerMaterial.h"
#include "CircuitExplorerAbstractRenderer.h"

// ospray
#include <ospray/SDK/common/Material.h>
#include <ospray/SDK/render/Renderer.h>

// system
#include <vector>

namespace circuitExplorer
{
/**
 * The CircuitExplorerSimulationRenderer class implements a parent renderer for
 * all Brayns renderers that need to render simulation data
 */
class CircuitExplorerSimulationRenderer : public CircuitExplorerAbstractRenderer
{
public:
    void commit() override;

protected:
    ospray::Model* _secondaryModel;
    float _maxDistanceToSecondaryModel{30.f};

    ospray::Ref<ospray::Data> _simulationData;
    ospray::uint64 _simulationDataSize;

    float _alphaCorrection{0.5f};

    float _fogThickness{1e6f};
    float _fogStart{0.f};
};
} // namespace circuitExplorer
