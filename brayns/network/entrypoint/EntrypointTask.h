/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include "EntrypointRequest.h"

#include <brayns/network/tasks/NetworkTask.h>

namespace brayns
{
template <typename ParamsType, typename ResultType>
class EntrypointTask : public NetworkTask
{
public:
    using Request = EntrypointRequest<ParamsType, ResultType>;

    EntrypointTask() = default;

    EntrypointTask(Request request)
        : _request(std::move(request))
    {
    }

    void execute(Request request)
    {
        cancel();
        wait();
        _request = std::move(request);
        start();
    }

    const ParamsType& getParams() const { return _request.getParams(); }

    void reply(const ResultType& result) { _request.reply(result); }

    virtual void onError(std::exception_ptr e) { _request.error(e); }

    virtual void onProgress(const std::string& operation, double amount)
    {
        _request.progress(operation, amount);
    }

private:
    Request _request;
};
} // namespace brayns