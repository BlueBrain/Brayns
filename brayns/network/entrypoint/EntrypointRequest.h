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

#include <brayns/network/socket/NetworkRequest.h>

namespace brayns
{
template <typename ParamsType, typename ResultType>
class EntrypointRequest
{
public:
    EntrypointRequest() = default;

    EntrypointRequest(NetworkRequest request)
        : _request(std::move(request))
    {
        Json::deserialize(request.getParams(), _params);
    }

    const ParamsType& getParams() const { return _params; }

    void error(int code, const std::string& message) const
    {
        _request.error(code, message);
    }

    void error(const std::string& message) const { _request.error(message); }

    void error(std::exception_ptr e) const { _request.error(e); }

    void progress(const std::string& operation, double amount) const
    {
        _request.progress(operation, amount);
    }

    void reply(const ResultType& result) const { _request.reply(result); }

private:
    NetworkRequest _request;
    ParamsType _params;
};
} // namespace brayns