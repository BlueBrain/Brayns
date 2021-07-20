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
#include "ObjectExtractor.h"

namespace brayns
{
template <typename ObjectType>
class GetEntrypoint : public BaseEntrypoint
{
public:
    virtual JsonSchema getParamsSchema() const override
    {
        return Json::getSchema<EmptyMessage>();
    }

    virtual JsonSchema getResultSchema() const override
    {
        auto& object = ObjectExtractor<ObjectType>::extract(getApi());
        return Json::getSchema(object);
    }

    virtual void onUpdate() const override
    {
        auto& object = ObjectExtractor<ObjectType>::extract(getApi());
        if (!object.isModified())
        {
            return;
        }
        notify(object);
    }

    virtual void onRequest(const NetworkRequest& request) const override
    {
        auto& object = ObjectExtractor<ObjectType>::extract(getApi());
        request.reply(object);
    }
};

template <typename ObjectType>
class SetEntrypoint : public BaseEntrypoint
{
public:
    virtual JsonSchema getParamsSchema() const override
    {
        auto& object = ObjectExtractor<ObjectType>::extract(getApi());
        return Json::getSchema(object);
    }

    virtual JsonSchema getResultSchema() const override
    {
        return Json::getSchema<EmptyMessage>();
    }

    virtual void onRequest(const NetworkRequest& request) const override
    {
        auto& params = request.getParams();
        auto& object = ObjectExtractor<ObjectType>::extract(getApi());
        Json::deserialize(params, object);
        triggerRender();
        request.reply(EmptyMessage());
    }
};
} // namespace brayns