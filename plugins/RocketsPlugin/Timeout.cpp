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

#include "Timeout.h"

namespace brayns
{
Timeout::~Timeout()
{
    clear();
}

void Timeout::set(const std::function<void()>& func, const int64_t wait)
{
    if (_timeout.valid())
        throw std::logic_error(
            "Timeout cannot be set() while it is still active");

    _cleared = false;
    _timeout = std::async(
        std::launch::async,
        [& mutex = _mutex, &condition = _condition, &cleared=_cleared, wait, func ] {
            std::unique_lock<std::mutex> lock(mutex);
            while(!cleared) // deals with spurious wakeups
            {
                if (condition.wait_for(lock, std::chrono::milliseconds(wait)) ==
                    std::cv_status::timeout)
                {
                    func();
                    break;
                }
            }
        });
};

void Timeout::clear()
{
    _cleared = true;
    if (_timeout.valid())
    {
        _condition.notify_one();
        _timeout.get();
    }
}
}
