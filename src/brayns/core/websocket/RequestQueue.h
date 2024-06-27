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

#include <condition_variable>
#include <functional>
#include <mutex>
#include <vector>

namespace brayns
{
using ClientId = std::uint32_t;

struct RawResponse
{
    std::string_view data;
    bool binary = false;
};

using ResponseHandler = std::function<void(const RawResponse &)>;

struct RawRequest
{
    ClientId clientId;
    std::string data;
    bool binary = false;
    ResponseHandler respond;
};

class RequestQueue
{
public:
    void push(RawRequest request);
    std::vector<RawRequest> wait();

private:
    std::mutex _mutex;
    std::condition_variable _condition;
    std::vector<RawRequest> _requests;
};
}
