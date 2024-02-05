/* Copyright 2015-2024 Blue Brain Project/EPFL
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
    /**
     * @brief Duration used to measure periods.
     *
     */
    using Duration = RateLimiter::Duration;

    /**
     * @brief Interactive notification period.
     *
     * @return Duration Equivalent duration.
     */
    static Duration interactive() { return std::chrono::milliseconds(1); }

    /**
     * @brief Default notification period.
     *
     * @return Duration Equivalent duration.
     */
    static Duration defaultValue() { return std::chrono::milliseconds(50); }

    /**
     * @brief Slow notification period.
     *
     * @return Duration Equivalent duration.
     */
    static Duration slow() { return std::chrono::milliseconds(750); }
};

/**
 * @brief Base class for entrypoints retrieving a Brayns object.
 *
 * Ex: get-camera, get-scene, get-application-parameters, etc...
 *
 * @tparam ObjectType Object type to retrieve.
 */
template <typename ObjectType>
class GetEntrypoint : public BaseEntrypoint
{
public:
    /**
     * @brief Duration used to rate limit notifications.
     *
     */
    using Duration = RateLimiter::Duration;

    /**
     * @brief Set the min duration between two notifications.
     *
     * @param duration Notification period.
     */
    void setNotificationPeriod(Duration duration) { _limiter = duration; }

    /**
     * @brief Get the object using ObjectExtractor<ObjectType>::extract(api).
     *
     * @return const ObjectType& Entrypoint object.
     */
    ObjectType& getObject() const
    {
        return ObjectExtractor<ObjectType>::extract(getApi());
    }

    /**
     * @brief Empty params schema.
     *
     * @return JsonSchema Entrypoint params schema.
     */
    virtual JsonSchema getParamsSchema() const override
    {
        return Json::getSchema<EmptyMessage>();
    }

    /**
     * @brief Result schema built from the object to retreive.
     *
     * @return JsonSchema Entrypoint result schema.
     */
    virtual JsonSchema getResultSchema() const override
    {
        auto& object = getObject();
        return Json::getSchema(object);
    }

    /**
     * @brief Setup object modification callback to notify it.
     *
     */
    virtual void onCreate() override
    {
        auto& object = getObject();
        object.onModified(
            [&](auto&) { _limiter.call([&] { notify(object); }); });
    }

    /**
     * @brief Reply the serialized object.
     *
     * @param request Client get-object request.
     */
    virtual void onRequest(const NetworkRequest& request) override
    {
        auto& object = getObject();
        request.reply(object);
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
    /**
     * @brief Extract the underlying object using ObjectExtractor::extract(api).
     *
     * @return ObjectType& Object to update.
     */
    ObjectType& getObject() const
    {
        return ObjectExtractor<ObjectType>::extract(getApi());
    }

    /**
     * @brief Build params schema using underlying object.
     *
     * @return JsonSchema Request params schema.
     */
    virtual JsonSchema getParamsSchema() const override
    {
        auto& object = getObject();
        return Json::getSchema(object);
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
     * @brief Update the object using the request.
     *
     * @param request Client set-object request.
     */
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