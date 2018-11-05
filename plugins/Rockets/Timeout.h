/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 *
 * Responsible Author: Daniel.Nachbaur@epfl.ch
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
#include <future>
#include <mutex>

namespace brayns
{
/**
 * Implements the setTimeout() and clearTimeout() mechanics from Javascript
 * (https://developer.mozilla.org/en-US/docs/Web/API/WindowOrWorkerGlobalScope/setTimeout),
 * following the same semantics.
 */
struct Timeout
{
    ~Timeout();
    void set(const std::function<void()>& func, const int64_t wait);
    void clear();

private:
    std::mutex _mutex;
    std::condition_variable _condition;
    std::future<void> _timeout;
    std::atomic_bool _cleared{true};
};
}
