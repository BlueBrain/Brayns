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
/**
 * @brief Wrapper around a network request strongly typed.
 *
 * @tparam ParamsType Params of the request.
 * @tparam ResultType Result of the request.
 */
template <typename ParamsType, typename ResultType>
class EntrypointRequest
{
public:
    EntrypointRequest() = default;

    EntrypointRequest(NetworkRequest request)
        : _request(std::move(request))
    {
    }

    const ConnectionHandle& getConnectionHandle() const
    {
        return _request.getConnectionHandle();
    }

    const std::string& getId() const { return _request.getId(); }

    const std::string& getMethod() const { return _request.getMethod(); }

    const JsonValue& getJsonParams() const { return _request.getParams(); }

    ParamsType getParams() const
    {
        auto& json = _request.getParams();
        return Json::deserialize<ParamsType>(json);
    }

    void getParams(ParamsType& params) const
    {
        auto& json = _request.getParams();
        Json::deserialize(json, params);
    }

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

    template <typename T>
    void notify(const T& params) const
    {
        _request.notify(params);
    }

private:
    NetworkRequest _request;
};
} // namespace brayns