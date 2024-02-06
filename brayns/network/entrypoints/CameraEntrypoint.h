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

#pragma once

#include <brayns/engine/core/Engine.h>
#include <brayns/engine/json/adapters/ProjectionAdapters.h>

#include <brayns/network/entrypoint/Entrypoint.h>

namespace brayns
{
class GetCameraTypeEntrypoint : public Entrypoint<EmptyJson, std::string>
{
public:
    explicit GetCameraTypeEntrypoint(Engine &engine);

    virtual std::string getMethod() const override;
    virtual std::string getDescription() const override;
    virtual void onRequest(const Request &request) override;

private:
    Engine &_engine;
};

template<typename T>
class SetCameraEntrypoint : public Entrypoint<T, EmptyJson>
{
public:
    using Request = typename Entrypoint<T, EmptyJson>::Request;

    explicit SetCameraEntrypoint(Engine &engine):
        _engine(engine)
    {
    }

    virtual void onRequest(const Request &request) override
    {
        auto &camera = _engine.getCamera();

        T projection;
        if (auto cast = camera.as<T>())
        {
            projection = *cast;
        }
        request.getParams(projection);
        camera.set(projection);
        request.reply(EmptyJson());
    }

private:
    Engine &_engine;
};

class SetCameraPerspectiveEntrypoint final : public SetCameraEntrypoint<Perspective>
{
public:
    explicit SetCameraPerspectiveEntrypoint(Engine &engine);

    std::string getMethod() const override;
    std::string getDescription() const override;
};

class SetCameraOrthographicEntrypoint final : public SetCameraEntrypoint<Orthographic>
{
public:
    explicit SetCameraOrthographicEntrypoint(Engine &engine);

    std::string getMethod() const override;
    std::string getDescription() const override;
};

template<typename T>
class GetCameraEntrypoint : public Entrypoint<EmptyJson, T>
{
public:
    using Request = typename Entrypoint<EmptyJson, T>::Request;

    explicit GetCameraEntrypoint(Engine &engine):
        _engine(engine)
    {
    }

    virtual void onRequest(const Request &request) override
    {
        auto &currentCamera = _engine.getCamera();
        if (auto castedProjection = currentCamera.as<T>())
        {
            request.reply(*castedProjection);
            return;
        }

        throw InvalidRequestException("Invalid camera type (should be '" + currentCamera.getName() + "')");
    }

private:
    Engine &_engine;
};

class GetCameraPerspectiveEntrypoint final : public GetCameraEntrypoint<Perspective>
{
public:
    explicit GetCameraPerspectiveEntrypoint(Engine &engine);

    std::string getMethod() const override;
    std::string getDescription() const override;
};

class GetCameraOrthographicEntrypoint final : public GetCameraEntrypoint<Orthographic>
{
public:
    explicit GetCameraOrthographicEntrypoint(Engine &engine);

    std::string getMethod() const override;
    std::string getDescription() const override;
};

} // namespace brayns
