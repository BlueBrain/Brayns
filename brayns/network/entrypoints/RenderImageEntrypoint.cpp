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

class ResultFormatter
{
public:
    static brayns::RenderImageResult format(const brayns::IRenderInterface &interface)
    {
        auto result = brayns::RenderImageResult();
        result.accumulation = interface.getAccumulation();
        result.max_accumulation = interface.getMaxAccumulation();
        return result;
    }
};
} // namespace

namespace brayns
{
RenderImageEntrypoint::RenderImageEntrypoint(
    const ApplicationParameters &parameters,
    std::unique_ptr<IRenderInterface> interface)
    : _parameters(parameters)
    , _interface(std::move(interface))
{
}

std::string RenderImageEntrypoint::getMethod() const
{
    return "render-image";
}

std::string RenderImageEntrypoint::getDescription() const
{
    return "Render an image and send it in JPEG format if required by params";
}

void RenderImageEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    ParamsValidator::validate(params);
    auto newFrame = _interface->render();
    auto result = ResultFormatter::format(*_interface);
    if (!SendingPolicy::mustSendJpeg(params, newFrame))
    {
        request.reply(result);
        return;
    }
    auto image = _interface->getCurrentFrame();
    auto quality = _parameters.getJpegQuality();
    auto jpeg = brayns::ImageEncoder::encode(image, "jpg", quality);
    request.reply(result, jpeg);
}
} // namespace brayns
