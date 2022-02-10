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

#include <brayns/common/adapters/PropertyMapAdapter.h>

#include <brayns/engine/Engine.h>

#include <brayns/network/common/PropertyObjectSchema.h>
#include <brayns/network/entrypoint/EntrypointNotifier.h>

#include "IEntrypoint.h"

namespace brayns
{
/**
 * @brief Base class for entrypoints retieving property objects
 *
 * Property objects have many property set available but holds only one at a
 * time.
 *
 * @tparam ObjectType Property object to retrieve.
 */
template<typename ObjectType>
class GetPropertyObjectEntrypoint : public IEntrypoint
{
public:
    /**
     * @brief Store the property object.
     *
     * @param object Object bound to the entrypoint.
     */
    GetPropertyObjectEntrypoint(const ObjectType &object)
        : _object(object)
    {
    }

    /**
     * @brief Empty params schema.
     *
     * @return JsonSchema Request params schema.
     */
    virtual JsonSchema getParamsSchema() const override
    {
        return Json::getSchema<EmptyMessage>();
    }

    /**
     * @brief Build oneOf schema using property set of object.
     *
     * @return JsonSchema Request result schema.
     */
    virtual JsonSchema getResultSchema() const override
    {
        return PropertyObjectSchema::create(_object);
    }

    /**
     * @brief Reply the current object properties.
     *
     * @param request Client get-...-properties request.
     */
    virtual void onRequest(const JsonRpcRequest &request) override
    {
        auto &properties = _object.getPropertyMap();
        auto result = Json::serialize(properties);
        request.reply(result);
    }

private:
    const ObjectType &_object;
};

/**
 * @brief Base class for entrypoints updating property objects.
 *
 * @tparam ObjectType Property object to update.
 */
template<typename ObjectType>
class SetPropertyObjectEntrypoint : public IEntrypoint
{
public:
    /**
     * @brief Store the object and the engine.
     *
     * @param object Object bound to the entrypoint.
     * @param engine Engine to trigger render when object is modified.
     * @param interface Interface to notify when object is modified.
     */
    SetPropertyObjectEntrypoint(ObjectType &object, Engine &engine, INetworkInterface &interface)
        : _object(object)
        , _engine(engine)
        , _notifier(*this, interface)
    {
    }

    /**
     * @brief OneOf request params schema from the object.
     *
     * @return JsonSchema Request params schema.
     */
    virtual JsonSchema getParamsSchema() const override
    {
        return PropertyObjectSchema::create(_object);
    }

    /**
     * @brief Empty result schema.
     *
     * @return JsonSchema Request result schema.
     */
    virtual JsonSchema getResultSchema() const override
    {
        return Json::getSchema<EmptyMessage>();
    }

    /**
     * @brief Update the object properties using request params.
     *
     * @param request Client set-...-properties request.
     */
    virtual void onRequest(const JsonRpcRequest &request) override
    {
        auto &params = request.getParams();
        auto properties = Json::deserialize<PropertyMap>(params);
        _object.updateProperties(properties);
        _engine.triggerRender();
        auto &properties = _object.getPropertyMap();
        _notifier.notify(properties);
        auto result = Json::serialize(EmptyMessage());
        request.reply(result);
    }

private:
    ObjectType &_object;
    Engine &_engine;
    EntrypointNotifier _notifier;
};
} // namespace brayns
