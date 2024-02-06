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

#include "CancellationToken.h"

namespace brayns
{
/**
 * @brief Helper class to notify a request progress and check cancellation.
 *
 * Request must have a progress(const std::string &, double) method.
 *
 * @tparam RequestType Type of request to monitor.
 */
template<typename RequestType>
class ProgressHandler
{
public:
    /**
     * @brief Construct a new progress handler for the given request.
     *
     * Reset token cancellation flag automatically.
     *
     * @param token Used to poll for cancellation.
     * @param request Request to monitor.
     */
    ProgressHandler(CancellationToken &token, const RequestType &request):
        _token(token),
        _request(request)
    {
        _token.reset();
    }

    /**
     * @brief Used to poll cancellation without sending notification.
     *
     * @throw TaskCancelledException Request has been cancelled.
     */
    void poll() const
    {
        _token.poll();
    }

    /**
     * @brief Poll cancellation and notifies request.
     *
     * @param operation Current operation description.
     * @param amount Current progress amount [0-1].
     * @throw TaskCancelledException Request has been cancelled.
     */
    void notify(const std::string &operation, double amount) const
    {
        _token.poll();
        _request.progress(operation, amount);
    }

private:
    CancellationToken &_token;
    const RequestType &_request;
};
} // namespace brayns
