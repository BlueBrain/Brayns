/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#pragma once

#include <brayns/network/entrypoint/Entrypoint.h>
#include <brayns/network/messages/RendererMessage.h>

#include <brayns/parameters/RenderingParameters.h>

namespace brayns
{
class GetRendererEntrypoint : public Entrypoint<EmptyMessage, RendererMessage>
{
public:
    virtual std::string getName() const override { return "get-renderer"; }

    virtual std::string getDescription() const override
    {
        return "Get the current state of the renderer";
    }

    virtual void onUpdate() const override
    {
        auto& manager = getApi().getParametersManager();
        auto& renderer = manager.getRenderingParameters();
        if (!renderer.isModified())
        {
            return;
        }
        auto params = _extractRenderer();
        notify(params);
    }

    virtual void onRequest(const Request& request) const override
    {
        auto result = _extractRenderer();
        request.reply(result);
    }

private:
    RendererMessage _extractRenderer() const
    {
        auto& manager = getApi().getParametersManager();
        auto& renderer = manager.getRenderingParameters();
        RendererMessage result;
        result.accumulation = renderer.getAccumulation();
        result.background_color = renderer.getBackgroundColor();
        result.current = renderer.getCurrentRenderer();
        result.head_light = renderer.getHeadLight();
        result.max_accum_frames = renderer.getMaxAccumFrames();
        result.samples_per_pixel = renderer.getSamplesPerPixel();
        result.subsampling = renderer.getSubsampling();
        result.types = renderer.getRenderers();
        result.variance_threshold = renderer.getVarianceThreshold();
        return result;
    }
};

class SetRendererEntrypoint : public Entrypoint<RendererMessage, EmptyMessage>
{
public:
    virtual std::string getName() const override { return "set-renderer"; }

    virtual std::string getDescription() const override
    {
        return "Set the current state of the renderer";
    }

    virtual JsonSchema getParamsSchema() const override
    {
        auto schema = RendererMessage::getSchema();
        JsonSchemaHelper::remove(schema, "types");
        return schema;
    }

    virtual void onRequest(const Request& request) const override
    {
        auto& params = request.getParams();
        auto& manager = getApi().getParametersManager();
        auto& renderer = manager.getRenderingParameters();
        renderer.setAccumulation(params.accumulation);
        renderer.setBackgroundColor(params.background_color);
        renderer.setCurrentRenderer(params.current);
        renderer.setHeadLight(params.head_light);
        renderer.setMaxAccumFrames(params.max_accum_frames);
        renderer.setSamplesPerPixel(params.samples_per_pixel);
        renderer.setSubsampling(params.subsampling);
        renderer.setVarianceThreshold(params.variance_threshold);
        triggerRender();
        request.reply(EmptyMessage());
    }
};
} // namespace brayns