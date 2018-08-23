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

#include "Timeout.h"

namespace brayns
{
/**
 * Executes the given function at most once every 'wait' milliseconds.
 *
 * Inspired by https://remysharp.com/2010/07/21/throttling-function-calls.
 */
struct Throttle
{
    void operator()(const std::function<void()>& fn, const int64_t wait = 100);

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> _last;
    bool _haveLast = false;
    Timeout _timeout;
};
}
