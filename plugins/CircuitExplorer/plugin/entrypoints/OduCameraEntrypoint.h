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

#include <plugin/messages/OduCameraMessage.h>

class GetOduCameraEntrypoint
    : public brayns::Entrypoint<brayns::EmptyMessage, OduCameraMessage>
{
public:
    virtual std::string getName() const override { return "get-odu-camera"; }

    virtual std::string getDescription() const override
    {
        return "Get the properties of the current camera";
    }

    virtual void onRequest(const Request& request) override
    {
        auto& camera = getApi().getCamera();
        OduCameraMessage result;
        auto& position = camera.getPosition();
        result.origin = position;
        auto& orientation = camera.getOrientation();
        result.direction = glm::rotate(orientation, {0.0, 0.0, -1.0});
        result.up = glm::rotate(orientation, {0.0, 1.0, 0.0});
        auto apertureRadius = camera.getPropertyOrValue("apertureRadius", 0.0);
        result.aperture_radius = apertureRadius;
        auto focusDistance = camera.getPropertyOrValue("focusDistance", 0.0);
        result.focus_distance = focusDistance;
        request.reply(result);
    }
};

class SetOduCameraEntrypoint
    : public brayns::Entrypoint<OduCameraMessage, brayns::EmptyMessage>
{
public:
    virtual std::string getName() const override { return "set-odu-camera"; }

    virtual std::string getDescription() const override
    {
        return "Set the properties of the current camera";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        auto& position = params.origin;
        auto& direction = params.direction;
        auto& up = params.up;
        auto target = position + direction;
        auto orientation = glm::lookAt(position, target, up);
        auto apertureRadius = params.aperture_radius;
        auto focusDistance = params.focus_distance;
        auto& camera = getApi().getCamera();
        camera.setPosition(position);
        camera.setTarget(target);
        camera.setOrientation(glm::inverse(orientation));
        camera.updateProperty("apertureRadius", apertureRadius);
        camera.updateProperty("focusDistance", focusDistance);
        triggerRender();
        request.reply(brayns::EmptyMessage());
    }
};