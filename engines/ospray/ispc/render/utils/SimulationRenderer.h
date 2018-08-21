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

#ifndef SIMULATIONRENDERER_H
#define SIMULATIONRENDERER_H

// obj
#include "../ExtendedOBJMaterial.h"
#include "AbstractRenderer.h"

// ospray
#include <ospray/SDK/common/Material.h>
#include <ospray/SDK/render/Renderer.h>

// system
#include <vector>

namespace brayns
{
/**
 * The SimulationRenderer class implements a parent renderer for all Brayns
 * renderers that need to render simulation data
 */
class SimulationRenderer : public AbstractRenderer
{
public:
    void commit() override;

protected:
    ospray::Ref<ospray::Data> _simulationData;
    ospray::uint64 _simulationDataSize;
    ospray::Ref<ospray::Data> _transferFunctionDiffuseData;
    ospray::Ref<ospray::Data> _transferFunctionEmissionData;
    float _transferFunctionMinValue;
    float _transferFunctionRange;
    ospray::int32 _transferFunctionSize;

    float _alphaCorrection;
};
}

#endif // SIMULATIONRENDERER_H
