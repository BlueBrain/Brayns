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

#include <brayns/core/json/Json.h>

#include "IEntrypoint.h"

namespace brayns
{
/**
 * @brief Base class for entrypoints retrieving a Brayns object.
 *
 * Ex: get-camera, get-scene, get-application-parameters, etc...
 *
 * @tparam ObjectType Object type to retrieve.
 */
template<typename ObjectType>
class GetEntrypoint : public IEntrypoint
{
public:
    /**
     * @brief Store the object and setup notifications.
     *
     * @param object Object bound to the entrypoint.
     */
    explicit GetEntrypoint(const ObjectType &object):
        _object(object)
    {
    }

    /**
     * @brief Empty params schema.
     *
     * @return JsonSchema Entrypoint params schema.
     */
    virtual JsonSchema getParamsSchema() const override
    {
        return Json::getSchema<EmptyJson>();
    }

    /**
     * @brief Result schema built from the object to retreive.
     *
     * @return JsonSchema Entrypoint result schema.
     */
    virtual JsonSchema getResultSchema() const override
    {
        return Json::getSchema<ObjectType>();
    }

    /**
     * @brief Reply the serialized object.
     *
     * @param request Client get-object request.
     */
    virtual void onRequest(const JsonRpcRequest &request) override
    {
        auto result = Json::serialize(_object);
        request.reply(result);
    }

private:
    const ObjectType &_object;
};

/**
 * @brief Base class for entrypoints updating Brayns objects.
 *
 * @tparam ObjectType Object type to update.
 */
template<typename ObjectType>
class SetEntrypoint : public IEntrypoint
{
public:
    /**
     * @brief Setup entrypoint with exposed object.
     *
     * @param object Object bound to the entrypoint.
     */
    explicit SetEntrypoint(ObjectType &object):
        _object(object)
    {
    }

    /**
     * @brief Build params schema using underlying object.
     *
     * @return JsonSchema Request params schema.
     */
    virtual JsonSchema getParamsSchema() const override
    {
        return Json::getSchema<ObjectType>();
    }

    /**
     * @brief Empty result schema.
     *
     * @return JsonSchema Request result schema.
     */
    virtual JsonSchema getResultSchema() const override
    {
        return Json::getSchema<EmptyJson>();
    }

    /**
     * @brief Update the object using the request.
     *
     * @param request Client set-object request.
     */
    virtual void onRequest(const JsonRpcRequest &request) override
    {
        auto &params = request.getParams();
        Json::deserialize(params, _object);
        auto result = Json::serialize(EmptyJson());
        request.reply(result);
    }

private:
    ObjectType &_object;
};
} // namespace brayns
