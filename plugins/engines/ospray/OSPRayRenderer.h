/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
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

#include <brayns/common/renderer/Renderer.h>
#include <brayns/common/types.h>

#include <ospray.h>

#include "OSPRayCamera.h"

namespace brayns
{
class OSPRayRenderer : public brayns::Renderer
{
public:
    OSPRayRenderer(const std::string& name,
                   ParametersManager& parametersMamager);
    ~OSPRayRenderer();

    void render(FrameBufferPtr frameBuffer) final;
    void commit() final;

    void setCamera(CameraPtr camera) final;

    const std::string& getName() const { return _name; }
    OSPRenderer impl() const { return _renderer; }
private:
    std::string _name;
    OSPRayCamera* _camera;
    OSPRenderer _renderer;
};
}

#endif // OSPRAYRENDERER_H
