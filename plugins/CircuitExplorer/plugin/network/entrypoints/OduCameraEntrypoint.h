/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
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

#pragma once

#include <brayns/engine/Camera.h>
#include <brayns/network/entrypoint/Entrypoint.h>

#include <plugin/api/CameraUtils.h>
#include <plugin/network/adapters/ODUCameraAdapter.h>

class GetOduCameraEntrypoint
    : public brayns::Entrypoint<brayns::EmptyMessage, ODUCameraInformation>
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
        request.reply(CameraUtils::getCameraAsODU(camera));
    }
};

class SetOduCameraEntrypoint
    : public brayns::Entrypoint<ODUCameraInformation, brayns::EmptyMessage>
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
        auto& camera = getApi().getCamera();
        CameraUtils::updateCamera(camera, params);
        triggerRender();
        request.reply(brayns::EmptyMessage());
    }
};
