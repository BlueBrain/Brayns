/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include "RenderImageEntrypoint.h"

#include <brayns/utils/image/ImageEncoder.h>

namespace
{
class ParamsValidator
{
public:
    static void validate(const brayns::RenderImageParams &params)
    {
        if (!params.send && params.force)
        {
            throw brayns::InvalidParamsException("Cannot force sending if send is disabled");
        }
    }
};

class SendingPolicy
{
public:
    static bool mustSendJpeg(const brayns::RenderImageParams &params, bool newFrame)
    {
        if (!params.send)
        {
            return false;
        }
        if (newFrame)
        {
            return true;
        }
        if (params.force)
        {
            return true;
        }
        return false;
    }
};

class RenderHelper
{
public:
    static bool render(brayns::Engine &engine)
    {
        engine.commitAndRender();
        auto &framebuffer = engine.getFramebuffer();
        auto newFrame = framebuffer.hasNewAccumulationFrame();
        framebuffer.resetNewAccumulationFrame();
        return newFrame;
    }
};

class ResultFormatter
{
public:
    static brayns::RenderImageResult format(brayns::Engine &engine)
    {
        auto result = brayns::RenderImageResult();
        result.accumulation = _getAccumulation(engine);
        result.max_accumulation = _getMaxAccumulation(engine);
        return result;
    }

private:
    static size_t _getAccumulation(brayns::Engine &engine)
    {
        auto &framebuffer = engine.getFramebuffer();
        return framebuffer.numAccumFrames();
    }

    static size_t _getMaxAccumulation(brayns::Engine &engine)
    {
        auto &renderer = engine.getRenderer();
        return renderer.getSamplesPerPixel();
    }
};

class JpegSerializer
{
public:
    static std::string serialize(brayns::Engine &engine)
    {
        auto &framebuffer = engine.getFramebuffer();
        auto image = framebuffer.getImage();
        auto &manager = engine.getParametersManager();
        auto &parameters = manager.getApplicationParameters();
        auto quality = parameters.getJpegQuality();
        return brayns::ImageEncoder::encode(image, "jpg", quality);
    }
};
} // namespace

namespace brayns
{
RenderImageEntrypoint::RenderImageEntrypoint(Engine &engine)
    : _engine(engine)
{
}

std::string RenderImageEntrypoint::getMethod() const
{
    return "render-image";
}

std::string RenderImageEntrypoint::getDescription() const
{
    return "Render an image and retreive it in JPEG format if required by params";
}

void RenderImageEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    ParamsValidator::validate(params);
    auto newFrame = RenderHelper::render(_engine);
    auto result = ResultFormatter::format(_engine);
    if (!SendingPolicy::mustSendJpeg(params, newFrame))
    {
        request.reply(result);
        return;
    }
    auto jpeg = JpegSerializer::serialize(_engine);
    request.reply(result, jpeg);
}
} // namespace brayns
