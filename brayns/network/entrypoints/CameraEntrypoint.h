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

#pragma once

#include <brayns/engine/Engine.h>
#include <brayns/json/JsonSchemaValidator.h>
#include <brayns/network/adapters/CameraAdapter.h>
#include <brayns/network/entrypoint/Entrypoint.h>
#include <brayns/utils/StringUtils.h>

namespace brayns
{
class GetCameraTypeEntrypoint : public Entrypoint<EmptyMessage, std::string>
{
public:
    GetCameraTypeEntrypoint(Engine &engine);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
    virtual void onRequest(const Request &request) override;

private:
    Engine &_engine;
};

template<typename T>
class SetCameraEntrypoint : public Entrypoint<T, EmptyMessage>
{
public:
    SetCameraEntrypoint(Engine &engine)
        : _engine(engine)
    {
    }

    virtual void onRequest(const typename Entrypoint<T, EmptyMessage>::Request &request) override
    {
        auto &currentCamera = _engine.getCamera();

        try
        {
            auto &castedCamera = dynamic_cast<T &>(currentCamera);
            request.getParams(castedCamera);
        }
        catch (...)
        {
            auto currentLookAt = currentCamera.getLookAt();

            auto newCamera = std::make_unique<T>();
            request.getParams(*newCamera);

            newCamera->setLookAt(currentLookAt);

            _engine.setCamera(std::move(newCamera));
        }

        request.reply(EmptyMessage());
    }

private:
    Engine &_engine;
};

class SetCameraPerspectiveEntrypoint final : public SetCameraEntrypoint<PerspectiveCamera>
{
public:
    SetCameraPerspectiveEntrypoint(Engine &engine);

    std::string getMethod() const override;
    std::string getDescription() const override;
};

class SetCameraOrthographicEntrypoint final : public SetCameraEntrypoint<OrthographicCamera>
{
public:
    SetCameraOrthographicEntrypoint(Engine &engine);

    std::string getMethod() const override;
    std::string getDescription() const override;
};

template<typename T>
class GetCameraEntrypoint : public Entrypoint<EmptyMessage, T>
{
public:
    GetCameraEntrypoint(Engine &engine)
        : _engine(engine)
    {
    }

    virtual void onRequest(const typename Entrypoint<EmptyMessage, T>::Request &request) override
    {
        auto &currentCamera = _engine.getCamera();
        try
        {
            auto &castedCamera = dynamic_cast<T &>(currentCamera);
            request.reply(castedCamera);
        }
        catch (const std::bad_cast &)
        {
            throw JsonRpcException("Cannot cast the current camera to the requested type");
        }
    }

private:
    Engine &_engine;
};

class GetCameraPerspectiveEntrypoint final : public GetCameraEntrypoint<PerspectiveCamera>
{
public:
    GetCameraPerspectiveEntrypoint(Engine &engine);

    std::string getMethod() const override;
    std::string getDescription() const override;
};

class GetCameraOrthographicEntrypoint final : public GetCameraEntrypoint<OrthographicCamera>
{
public:
    GetCameraOrthographicEntrypoint(Engine &engine);

    std::string getMethod() const override;
    std::string getDescription() const override;
};

} // namespace brayns
