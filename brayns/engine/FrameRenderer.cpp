/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <brayns/engine/FrameRenderer.h>

#include <ospray/ospray_util.h>

namespace brayns
{
void FrameRenderer::synchronous(const Camera &cam, const FrameBuffer &fb, const Renderer &render, const Scene &scene)
{
    auto cameraHandle = cam.handle();
    auto fbHandle = fb.handle();
    auto rendererHandle = render.handle();
    auto sceneHandle = scene.handle();

    ospRenderFrameBlocking(fbHandle, rendererHandle, cameraHandle, sceneHandle);
}

OSPFuture FrameRenderer::asynchronous(const Camera& camera,
                                      const FrameBuffer& fb,
                                      const Renderer& renderer,
                                      const Scene& scene)
{
    auto cameraHandle = camera.handle();
    auto fbHandle = fb.handle();
    auto rendererHandle = renderer.handle();
    auto sceneHandle = scene.handle();

    auto frameFuture = ospRenderFrame(fbHandle, rendererHandle, cameraHandle, sceneHandle);
    return frameFuture;
}
}
