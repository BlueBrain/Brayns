/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include <plugins/engines/ospray/ispc/render/ExtendedOBJRenderer.h>

// ispc exports
#include "ExtendedOBJRenderer_ispc.h"

#define OSP_REGISTER_EXRENDERER(InternalClassName, external_name)          \
    extern "C" ospray::Renderer* ospray_create_renderer__##external_name() \
    {                                                                      \
        return new InternalClassName;                                      \
    }

namespace brayns
{
void ExtendedOBJRenderer::commit()
{
    AbstractRenderer::commit();
    ispc::ExtendedOBJRenderer_set(getIE(), (ispc::vec3f&)_bgColor,
                                  _shadowsEnabled, _softShadowsEnabled,
                                  _ambientOcclusionStrength, _shadingEnabled,
                                  _randomNumber, _timestamp, _spp,
                                  _electronShadingEnabled, _lightPtr,
                                  _lightArray.size(), _materialPtr,
                                  _materialArray.size());
}

ExtendedOBJRenderer::ExtendedOBJRenderer()
{
    ispcEquivalent = ispc::ExtendedOBJRenderer_create(this);
}

OSP_REGISTER_EXRENDERER(ExtendedOBJRenderer, EXOBJ);
OSP_REGISTER_EXRENDERER(ExtendedOBJRenderer, exobj);
} // ::brayns
