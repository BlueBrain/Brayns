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

#pragma once

#include <brayns/engine/Camera.h>
#include <brayns/engine/FrameBuffer.h>
#include <brayns/engine/Renderer.h>
#include <brayns/engine/Scene.h>

namespace brayns
{
struct FrameRenderer
{
    /**
     * @brief Renders a frame synchronously. When the function returns, the rendering process will have finished.
     * @param camera
     * @param fb
     * @param renderer
     * @param scene
     */
    static void synchronous(const Camera &camera, const FrameBuffer &fb, const Renderer &renderer, const Scene &scene);

    /**
     * @brief Renders a frame asynchronously. The function returns inmediatly with an OSPFuture, which can be used
     * for both querying the current frame rendering process and waiting for the frame rendering to complete.
     * @param camera
     * @param fb
     * @param renderer
     * @param scene
     * @return OSPFuture
     */
    static OSPFuture asynchronous(const Camera &cam, const FrameBuffer &fb, const Renderer &render, const Scene &scen);
};
}
