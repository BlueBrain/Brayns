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

#pragma once
#include "OptiXCamera.h"

#include <brayns/common/Timer.h>
#include <brayns/common/types.h>
#include <brayns/engineapi/Renderer.h>

namespace brayns
{
/**
   OptiX specific renderer

   This object is the OptiX specific implementation of a renderer
*/
class OptiXRenderer : public Renderer
{
public:
    OptiXRenderer(const AnimationParameters& animationParameters,
                  const RenderingParameters& renderingParameters);

    void render(FrameBufferPtr frameBuffer) final;

    void commit() final;

    void setCamera(CameraPtr camera) final;

private:
    std::string _currentRenderer;

    Timer _timer;
};
} // namespace brayns
