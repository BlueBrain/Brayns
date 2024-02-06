/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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
    static bool mustSendImage(const brayns::RenderImageParams &params, bool newFrame)
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
    static bool render(brayns::Engine &engine, const brayns::RenderImageParams &params)
    {
        if (!params.render)
        {
            return false;
        }
        if (!params.accumulate)
        {
            return render(engine);
        }
        auto frameCount = 0;
        while (render(engine))
        {
            ++frameCount;
        }
        return frameCount > 0;
    }

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
        return framebuffer.getAccumulationFrameCount();
    }

    static size_t _getMaxAccumulation(brayns::Engine &engine)
    {
        auto &renderer = engine.getRenderer();
        return renderer.getSamplesPerPixel();
    }
};

class ImageSerializer
{
public:
    static std::string serialize(brayns::Engine &engine, const brayns::RenderImageEntrypoint::Params &params)
    {
        auto &framebuffer = engine.getFramebuffer();
        if (!framebuffer.getHandle())
        {
            return {};
        }
        auto image = framebuffer.getImage();
        auto format = params.format;
        auto quality = params.jpeg_quality;
        return brayns::ImageEncoder::encode(image, format, quality);
    }
};
} // namespace

namespace brayns
{
RenderImageEntrypoint::RenderImageEntrypoint(Engine &engine):
    _engine(engine)
{
}

std::string RenderImageEntrypoint::getMethod() const
{
    return "render-image";
}

std::string RenderImageEntrypoint::getDescription() const
{
    return "Render an image of the current context and retreive it according to given params";
}

void RenderImageEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    ParamsValidator::validate(params);
    auto newFrame = RenderHelper::render(_engine, params);
    auto result = ResultFormatter::format(_engine);
    if (!SendingPolicy::mustSendImage(params, newFrame))
    {
        request.reply(result);
        return;
    }
    auto image = ImageSerializer::serialize(_engine, params);
    request.reply(result, image);
}
} // namespace brayns
