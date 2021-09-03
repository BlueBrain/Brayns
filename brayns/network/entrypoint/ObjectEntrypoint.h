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

#include <brayns/network/common/RateLimiter.h>

#include "BaseEntrypoint.h"

namespace brayns
{
/**
 * @brief Template class used to extract an object from PluginAPI.
 *
 * Must have a static method T& extract(PluginAPI& api).
 *
 * @tparam T Object type to extract.
 */
template <typename T>
struct ObjectExtractor
{
};

/**
 * @brief Available notification periods.
 *
 */
struct NotificationPeriod
{
    using Duration = RateLimiter::Duration;

    static Duration interactive() { return std::chrono::milliseconds(1); }
    static Duration defaultValue() { return std::chrono::milliseconds(50); }
    static Duration slow() { return std::chrono::milliseconds(750); }
};

/**
 * @brief Base class for entrypoints retrieving a Brayns object (camera,
 * renderer, ...).
 *
 * @tparam ObjectType Object type to retrieve.
 */
template <typename ObjectType>
class GetEntrypoint : public BaseEntrypoint
{
public:
    using Duration = RateLimiter::Duration;

    void setNotificationPeriod(Duration duration) { _limiter = duration; }

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

    virtual void onRequest(const NetworkRequest& request) override
    {
        auto& object = getObject();
        request.reply(object);
    }

    virtual void onPostRender() override
    {
        auto& object = getObject();
        if (!object.isModified())
        {
            return;
        }
        _limiter.call([&] { notify(object); });
    }

private:
    RateLimiter _limiter = NotificationPeriod::defaultValue();
};

/**
 * @brief Base class for entrypoints updating Brayns objects.
 *
 * @tparam ObjectType Object type to update.
 */
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