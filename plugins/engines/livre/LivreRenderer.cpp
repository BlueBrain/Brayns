/* Copyright (c) 2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel.Nachbaur@epfl.ch
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

#include "LivreRenderer.h"
#include "LivreFrameBuffer.h"

#include <brayns/common/scene/Scene.h>
#include <brayns/common/volume/VolumeHandler.h>

#include <livre/eq/Engine.h>
#include <livre/eq/FrameData.h>
#include <livre/eq/settings/FrameSettings.h>
#include <livre/lib/configuration/ApplicationParameters.h>

namespace brayns
{
LivreRenderer::LivreRenderer(ParametersManager& parametersManager,
                             livre::Engine& livre)
    : Renderer(parametersManager)
    , _livre(livre)
{
}

void LivreRenderer::render(FrameBufferPtr frameBuffer)
{
    _livre.render([=](const uint32_t /*frameNumber*/, const eq::Image& image) {
        auto buffer = std::dynamic_pointer_cast<LivreFrameBuffer>(frameBuffer);
        if (buffer)
            buffer->assign(image);
    });
}

void LivreRenderer::commit()
{
    auto& livreSettings = _livre.getFrameData().getFrameSettings();
    const auto& livreParams = _livre.getApplicationParameters();

    const uint64_t nbFrames = livreParams.frames[1] - livreParams.frames[0];
    const uint64_t ts = _parametersManager.getSceneParameters().getTimestamp();

    livreSettings.setFrameNumber(nbFrames == 0 ? 0 : (ts % nbFrames));

    if (_scene->getVolumeHandler())
        _scene->getVolumeHandler()->setNbFrames(nbFrames);
}

void LivreRenderer::setCamera(CameraPtr /*camera*/)
{
}
}
