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
#include <brayns/network/messages/CameraMessage.h>

#include <brayns/engine/Camera.h>

namespace brayns
{
class GetCameraEntrypoint : public Entrypoint<EmptyMessage, CameraMessage>
{
public:
    virtual std::string getName() const override { return "get-camera"; }

    virtual std::string getDescription() const override
    {
        return "Get the current state of the camera";
    }

    virtual void onUpdate() const override
    {
        auto& engine = getApi().getEngine();
        auto& camera = engine.getCamera();
        if (!camera.isModified())
        {
            return;
        }
        auto params = _extractCamera();
        notify(params);
    }

    virtual void onRequest(const Request& request) const override
    {
        auto result = _extractCamera();
        request.reply(result);
    }

private:
    CameraMessage _extractCamera() const
    {
        auto& engine = getApi().getEngine();
        auto& camera = engine.getCamera();
        CameraMessage result;
        result.orientation = camera.getOrientation();
        result.position = camera.getPosition();
        result.target = camera.getTarget();
        result.current = camera.getCurrentType();
        result.types = camera.getTypes();
        return result;
    }
};

class SetCameraEntrypoint : public Entrypoint<CameraMessage, EmptyMessage>
{
public:
    virtual std::string getName() const override { return "set-camera"; }

    virtual std::string getDescription() const override
    {
        return "Set the current state of the camera";
    }

    virtual JsonSchema getParamsSchema() const override
    {
        auto schema = CameraMessage::getSchema();
        JsonSchemaHelper::remove(schema, "types");
        return schema;
    }

    virtual void onRequest(const Request& request) const override
    {
        auto& params = request.getParams();
        auto& engine = getApi().getEngine();
        auto& camera = engine.getCamera();
        camera.setOrientation(params.orientation);
        camera.setPosition(params.position);
        camera.setTarget(params.target);
        camera.setCurrentType(params.current);
        request.reply(EmptyMessage());
    }
};
} // namespace brayns