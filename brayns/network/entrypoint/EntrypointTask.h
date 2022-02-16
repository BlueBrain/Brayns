/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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
#include <brayns/network/tasks/NetworkTask.h>

#include "EntrypointRequest.h"

namespace brayns
{
/**
 * @brief Helper class to run an entrypoint request in a separated thread.
 *
 * @tparam ParamsType Entrypoint request params.
 * @tparam ResultType Entrypoint request result.
 */
template<typename ParamsType, typename ResultType>
class EntrypointTask : public NetworkTask
{
public:
    /**
     * @brief Alias for corresponding EntrypointRequest.
     *
     */
    using Request = EntrypointRequest<ParamsType, ResultType>;

    /**
     * @brief Construct a task with request.
     *
     * Store the request to allow subclasses to access it.
     *
     * @param request Request starting the task.
     */
    EntrypointTask(Request request)
        : _request(std::move(request))
    {
    }

    /**
     * @brief Get the client that started the task.
     *
     * @return const ClientRef& Client ref.
     */
    const ClientRef &getClient() const
    {
        return _request.getClient();
    }

    /**
     * @brief Get the ID of the request that started the task.
     *
     * @return const RequestId& Request ID.
     */
    const RequestId &getRequestId() const
    {
        return _request.getId();
    }

    /**
     * @brief Parse and return the request params.
     *
     * @return ParamsType Request params.
     */
    ParamsType getParams() const
    {
        return _request.getParams();
    }

    /**
     * @brief Parse the request params to the given instance.
     *
     * @param params Request params.
     */
    void getParams(ParamsType &params) const
    {
        return _request.getParams(params);
    }

protected:
    /**
     * @brief Send success reply using underlying request.
     *
     * @param result Reply result data.
     */
    void reply(const ResultType &result)
    {
        _request.reply(result);
    }

    /**
     * @brief Send an error reply when an exception occurs in the thread.
     *
     * @param e Source of the error.
     */
    virtual void onError(const JsonRpcException &e) override
    {
        _request.error(e);
    }

    /**
     * @brief Send progress message when progress() is called.
     *
     * A rate limit of 1 per second is applied.
     *
     * @param operation Current operation.
     * @param amount Progress amount 0-1.
     */
    virtual void onProgress(const std::string &operation, double amount) override
    {
        _limiter.call([&] { _request.progress(operation, amount); });
    }

private:
    Request _request;
    RateLimiter _limiter{std::chrono::seconds(1)};
};
} // namespace brayns
