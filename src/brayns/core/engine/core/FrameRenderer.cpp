/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "FrameRenderer.h"

#include <ospray/ospray_util.h>

namespace brayns
{
void FrameRenderer::synchronous(
    const Camera &camera,
    const Framebuffer &framebuffer,
    const Renderer &renderer,
    const Scene &scene)
{
    auto future = asynchronous(camera, framebuffer, renderer, scene);
    future.wait();
}

ospray::cpp::Future FrameRenderer::asynchronous(
    const Camera &camera,
    const Framebuffer &framebuffer,
    const Renderer &renderer,
    const Scene &scene)
{
    auto &osprayCamera = camera.getHandle();
    auto &osprayFrameBuffer = framebuffer.getHandle();
    auto &osprayRenderer = renderer.getHandle();
    auto &osprayScene = scene.getHandle();

    auto future = osprayFrameBuffer.renderFrame(osprayRenderer, osprayCamera, osprayScene);
    return future;
}
}
