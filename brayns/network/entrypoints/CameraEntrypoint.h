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
#include <brayns/network/messages/LookAtMessage.h>
#include <brayns/utils/StringUtils.h>

namespace brayns
{
template<typename T>
class GetCameraEntrypoint : public IEntrypoint
{
public:
    GetCameraEntrypoint(Engine &engine)
        : _engine(engine)
    {
    }

    virtual JsonSchema getParamsSchema() const override
    {
        return Json::getSchema<EmptyMessage>();
    }

    virtual JsonSchema getResultSchema() const override
    {
        return Json::getSchema<T>();
    }

    virtual void onRequest(const JsonRpcRequest &request) override
    {
        auto& currentCamera = _engine.getCamera();
        try
        {
            auto& castedCamera = dynamic_cast<T&>(currentCamera);
            const auto result = Json::serialize<T>(castedCamera);
            request.reply(result);
        }
        catch(const std::bad_cast&)
        {
            throw JsonRpcException("Cannot cast system camera to the requested type");
        }
    }

private:
    Engine &_engine;
};

template<typename T>
class SetCameraEntrypoint : public IEntrypoint
{
public:
    SetCameraEntrypoint(Engine &engine)
        : _engine(engine)
    {
    }

    virtual JsonSchema getParamsSchema() const override
    {
        return Json::getSchema<T>();
    }

    virtual JsonSchema getResultSchema() const override
    {
        return Json::getSchema<EmptyMessage>();
    }

    virtual void onRequest(const JsonRpcRequest &request) override
    {
        const auto params = request.getParams();

        const auto errors = JsonSchemaValidator::validate(params, getParamsSchema());
        if (!errors.empty())
        {
            throw JsonRpcException("Could not parse camera parameters: " + string_utils::join(errors, ", "));
        }

        auto newCamera = Json::deserialize<std::unique_ptr<T>>(params);
        _engine.setCamera(std::move(newCamera));
        const auto result = Json::serialize(EmptyMessage());
        request.reply(result);
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

class GetCameraOrthographicEntrypoit final : public GetCameraEntrypoint<OrthographicCamera>
{
public:
    GetCameraOrthographicEntrypoit(Engine &engine);

    std::string getMethod() const override;
    std::string getDescription() const override;
};

class SetCameraPerspectiveEntrypoint final : public SetCameraEntrypoint<PerspectiveCamera>
{
public:
    SetCameraPerspectiveEntrypoint(Engine &engine);

    std::string getMethod() const override;
    std::string getDescription() const override;
};

class SetCameraOrthographicEntrypoit final : public SetCameraEntrypoint<OrthographicCamera>
{
public:
    SetCameraOrthographicEntrypoit(Engine &engine);

    std::string getMethod() const override;
    std::string getDescription() const override;
};

class GetCurrentCameraTypeEntrypoint : public Entrypoint<EmptyMessage, std::string>
{
public:
    GetCurrentCameraTypeEntrypoint(Engine& engine);

    std::string getMethod() const override;
    std::string getDescription() const override;
    void onRequest(const Request &request) override;

private:
    Engine& _engine;
};

class CameraLookAtEntrypoint final : public Entrypoint<LookAtParameters, EmptyMessage>
{
public:
    CameraLookAtEntrypoint(Engine &engine);

    std::string getMethod() const override;
    std::string getDescription() const override;
    void onRequest(const Request &request) override;

private:
    Engine &_engine;
};

} // namespace brayns
