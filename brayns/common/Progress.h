/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/common/BaseObject.h>

#include <functional>
#include <mutex>
#include <string>

namespace brayns
{
/**
 * A progress object which offers thread-safe progress updates and thread-safe
 * consumption of the current progress if it has changed in between.
 */
class Progress : public BaseObject
{
public:
    Progress() = default;
    explicit Progress(const std::string& operation)
        : _operation(operation)
    {
    }

    /** Update the progress with a new absolute amount. */
    void update(const std::string& operation, const float amount)
    {
        std::lock_guard<std::mutex> lock_(_mutex);
        _updateValue(_operation, operation);
        _updateValue(_amount, amount);
    }

    /** Update the progress with the given increment. */
    void increment(const std::string& operation, const float increment)
    {
        std::lock_guard<std::mutex> lock_(_mutex);
        _updateValue(_operation, operation);
        _updateValue(_amount, _amount + increment);
    }

    /**
     * Call the provided callback with the current progress if it has changed
     * since the last invokation.
     */
    void consume(std::function<void(std::string, float)> callback)
    {
        std::lock_guard<std::mutex> lock_(_mutex);
        if (isModified())
        {
            callback(_operation, _amount);
            resetModified();
        }
    }

private:
    std::string _operation;
    float _amount{0.f};
    std::mutex _mutex;
};
} // namespace brayns
