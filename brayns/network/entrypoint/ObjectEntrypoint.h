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

#include "BaseEntrypoint.h"

namespace brayns
{
template <typename T>
struct ObjectExtractor
{
};

template <typename ObjectType>
class GetEntrypoint : public BaseEntrypoint
{
public:
    const ObjectType& getObject() const
    {
        return ObjectExtractor<ObjectType>::extract(getApi());
    }

    virtual JsonSchema getParamsSchema() const override
    {
        return Json::getSchema<EmptyMessage>();
    }

    virtual JsonSchema getResultSchema() const override
    {
        auto& object = getObject();
        return Json::getSchema(object);
    }

    virtual void onUpdate() override
    {
        auto& object = getObject();
        if (!object.isModified())
        {
            return;
        }
        notify(object);
    }

    virtual void onRequest(const NetworkRequest& request) override
    {
        auto& object = getObject();
        request.reply(object);
    }
};

template <typename ObjectType>
class SetEntrypoint : public BaseEntrypoint
{
public:
    ObjectType& getObject() const
    {
        return ObjectExtractor<ObjectType>::extract(getApi());
    }

    virtual JsonSchema getParamsSchema() const override
    {
        auto& object = getObject();
        return Json::getSchema(object);
    }

    virtual JsonSchema getResultSchema() const override
    {
        return Json::getSchema<EmptyMessage>();
    }

    virtual void onRequest(const NetworkRequest& request) override
    {
        auto& params = request.getParams();
        auto& object = getObject();
        Json::deserialize(params, object);
        triggerRender();
        request.reply(EmptyMessage());
    }
};
} // namespace brayns