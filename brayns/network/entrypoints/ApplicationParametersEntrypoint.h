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
#include <brayns/network/messages/ApplicationParametersMessage.h>

#include <brayns/parameters/ApplicationParameters.h>

namespace brayns
{
class GetApplicationParametersEntrypoint
    : public Entrypoint<EmptyMessage, ApplicationParametersMessage>
{
public:
    virtual std::string getName() const override
    {
        return "get-application-parameters";
    }

    virtual std::string getDescription() const override
    {
        return "Get the current state of the application parameters";
    }

    virtual void onUpdate() const override
    {
        auto& manager = getApi().getParametersManager();
        auto& applicationParameters = manager.getApplicationParameters();
        if (!applicationParameters.isModified())
        {
            return;
        }
        auto params = _extractApplicationParameters();
        notify(params);
    }

    virtual void onRequest(const Request& request) const override
    {
        auto result = _extractApplicationParameters();
        request.reply(result);
    }

private:
    ApplicationParametersMessage _extractApplicationParameters() const
    {
        auto& manager = getApi().getParametersManager();
        auto& applicationParameters = manager.getApplicationParameters();
        ApplicationParametersMessage result;
        result.engine = applicationParameters.getEngine();
        result.plugins = applicationParameters.getPlugins();
        result.jpeg_compression = applicationParameters.getJpegCompression();
        result.image_stream_fps = applicationParameters.getImageStreamFPS();
        result.viewport = applicationParameters.getWindowSize();
        return result;
    }
};

class SetApplicationParametersEntrypoint
    : public Entrypoint<ApplicationParametersMessage, EmptyMessage>
{
public:
    virtual std::string getName() const override
    {
        return "set-application-parameters";
    }

    virtual std::string getDescription() const override
    {
        return "Set the current state of the application parameters";
    }

    virtual JsonSchema getParamsSchema() const override
    {
        auto schema = Base::getParamsSchema();
        JsonProperty::remove(schema, "engine");
        JsonProperty::remove(schema, "plugins");
        return schema;
    }

    virtual void onRequest(const Request& request) const override
    {
        auto& params = request.getParams();
        auto& manager = getApi().getParametersManager();
        auto& applicationParameters = manager.getApplicationParameters();
        applicationParameters.setJpegCompression(params.jpeg_compression);
        applicationParameters.setImageStreamFPS(params.image_stream_fps);
        applicationParameters.setWindowSize(params.viewport);
        triggerRender();
        request.reply(EmptyMessage());
    }
};
} // namespace brayns