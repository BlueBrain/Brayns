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

#ifndef OSPRAYRENDERER_H
#define OSPRAYRENDERER_H

#include <brayns/common/types.h>
#include <brayns/engineapi/Renderer.h>

#include <ospray.h>

#include "OSPRayCamera.h"

namespace brayns
{
class OSPRayRenderer : public Renderer
{
public:
    OSPRayRenderer(const AnimationParameters& animationParameters,
                   const RenderingParameters& renderingParameters);
    ~OSPRayRenderer();

    void render(FrameBufferPtr frameBuffer) final;
    void commit() final;
    float getVariance() const final { return _variance; }
    void setCamera(CameraPtr camera) final;

    PickResult pick(const Vector2f& pickPos) final;

    void setClipPlanes(const Planes& planes);

private:
    OSPRayCamera* _camera{nullptr};
    OSPRenderer _renderer{nullptr};
    std::atomic<float> _variance{std::numeric_limits<float>::max()};
    std::string _currentOSPRenderer;
    OSPData _currLightsData{nullptr};

    Planes _clipPlanes;

    void _createOSPRenderer();
    void _commitRendererMaterials();
    void _destroyRenderer();
};
} // namespace brayns

#endif // OSPRAYRENDERER_H
