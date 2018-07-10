/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Authors: Samuel Lapere <samuel.lapere@epfl.ch>
 *                      Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * Based on OSPRay implementation
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

#include <plugins/engines/ospray/ispc/render/PathTracingRenderer.h>

// ispc exports
#include "PathTracingRenderer_ispc.h"

#define OSP_REGISTER_EXRENDERER(InternalClassName, external_name)          \
    extern "C" ospray::Renderer* ospray_create_renderer__##external_name() \
    {                                                                      \
        return new InternalClassName;                                      \
    }

namespace brayns
{
void PathTracingRenderer::commit()
{
    AbstractRenderer::commit();

    _shadows = getParam1f("shadows", 0.f);
    _softShadows = getParam1f("softShadows", 0.f);
    _ambientOcclusionStrength = getParam1f("aoWeight", 0.f);
    _ambientOcclusionDistance = getParam1f("aoDistance", 1e20f);

    ispc::PathTracingRenderer_set(
        getIE(), (_bgMaterial ? _bgMaterial->getIE() : nullptr), _timestamp,
        spp, _lightPtr, _lightArray.size(), _shadows, _softShadows,
        _ambientOcclusionStrength, _ambientOcclusionDistance);
}

PathTracingRenderer::PathTracingRenderer()
{
    ispcEquivalent = ispc::PathTracingRenderer_create(this);
}

OSP_REGISTER_EXRENDERER(PathTracingRenderer, pathtracingrenderer);
} // ::brayns
