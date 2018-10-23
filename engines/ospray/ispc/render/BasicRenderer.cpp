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

#include "BasicRenderer.h"

// ispc exports
#include "BasicRenderer_ispc.h"

namespace brayns
{
void BasicRenderer::commit()
{
    AbstractRenderer::commit();
    ispc::BasicRenderer_set(getIE(),
                            (_bgMaterial ? _bgMaterial->getIE() : nullptr),
                            _timestamp, spp, _lightPtr, _lightArray.size());
}

BasicRenderer::BasicRenderer()
{
    ispcEquivalent = ispc::BasicRenderer_create(this);
}

OSP_REGISTER_RENDERER(BasicRenderer, BASIC);
OSP_REGISTER_RENDERER(BasicRenderer, basic);
} // ::brayns
