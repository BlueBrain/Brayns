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
#include <brayns/network/adapters/RendererAdapter.h>
#include <brayns/network/entrypoint/Entrypoint.h>
#include <brayns/utils/StringUtils.h>

namespace brayns
{
class GetRendererTypeEntrypoint final : public Entrypoint<EmptyMessage, std::string>
{
public:
    GetRendererTypeEntrypoint(Engine &engine);

    std::string getMethod() const override;
    std::string getDescription() const override;

    void onRequest(const Request &request) override;

private:
    Engine &_engine;
};

template<typename T>
class SetRendererEntrypoint : public Entrypoint<T, EmptyMessage>
{
public:
    SetRendererEntrypoint(Engine &engine)
        : _engine(engine)
    {
    }

    virtual void onRequest(const typename Entrypoint<T, EmptyMessage>::Request &request) override
    {
        T* targetRenderer = nullptr;
        try
        {
            auto &renderer = _engine.getRenderer();
            auto &castedRenderer = dynamic_cast<T&>(renderer);
            targetRenderer = &castedRenderer;
        }
        catch(const std::bad_cast &)
        {
            auto newRenderer = std::make_unique<T>();
            targetRenderer = newRenderer.get();
            _engine.setRenderer(std::move(newRenderer));
        }

        request.getParams(*targetRenderer);

        request.reply(EmptyMessage());
    }

private:
    Engine &_engine;
};

class SetRendererInteractiveEntrypoint final : public SetRendererEntrypoint<InteractiveRenderer>
{
public:
    SetRendererInteractiveEntrypoint(Engine &engine);

    std::string getMethod() const override;
    std::string getDescription() const override;
};

class SetRendererProductionEntrypoint final : public SetRendererEntrypoint<ProductionRenderer>
{
public:
    SetRendererProductionEntrypoint(Engine &engine);

    std::string getMethod() const override;
    std::string getDescription() const override;
};

template<typename T>
class GetRendererEntrypoint : public Entrypoint<EmptyMessage, T>
{
public:
    GetRendererEntrypoint(Engine& engine)
     : _engine(engine)
    {
    }

    void onRequest(const typename Entrypoint<EmptyMessage, T>::Request &request) override
    {
        auto &renderer = _engine.getRenderer();
        try
        {
            auto &castedRenderer = dynamic_cast<T&>(renderer);
            request.reply(castedRenderer);
        }
        catch(const std::bad_cast &)
        {
            throw JsonRpcException("Cannot cast the renderer to the requested type");
        }
    }

private:
    Engine &_engine;
};

class GetRendererInteractiveEntrypoint final : public GetRendererEntrypoint<InteractiveRenderer>
{
public:
    GetRendererInteractiveEntrypoint(Engine &engine);

    std::string getMethod() const override;
    std::string getDescription() const override;
};

class GetRendererProductionEntrypoint final : public GetRendererEntrypoint<ProductionRenderer>
{
public:
    GetRendererProductionEntrypoint(Engine &engine);

    std::string getMethod() const override;
    std::string getDescription() const override;
};
} // namespace brayns
