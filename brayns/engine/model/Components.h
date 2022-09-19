/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <algorithm>
#include <any>
#include <cassert>
#include <typeindex>
#include <vector>

namespace brayns
{
class Components
{
public:
    template<typename T, typename... Args>
    T &add(Args &&...args)
    {
        static_assert(std::is_same_v<std::decay_t<T>, T>, "Only decayed components allowed");

        auto type = std::type_index(typeid(T));

        if constexpr (std::is_aggregate_v<T>)
        {
            _components.push_back(Entry{type, T{std::forward<Args>(args)...}});
        }
        else
        {
            _components.push_back(Entry{type, T(std::forward<Args>(args)...)});
        }

        return std::any_cast<T &>(_components.back().component);
    }

    template<typename T>
    T &getOrAdd()
    {
        if (auto component = find<T>())
        {
            return *component;
        }
        return add<T>();
    }

    template<typename T>
    T &get()
    {
        auto component = find<T>();
        assert(component);
        return *component;
    }

    template<typename T>
    const T &get() const
    {
        auto component = find<T>();
        assert(component);
        return *component;
    }

    template<typename T>
    T *find()
    {
        auto it = _findIterator<T>();
        if (it == _components.end())
        {
            return nullptr;
        }
        auto &entry = *it;
        auto &component = entry.component;
        return std::any_cast<T>(&component);
    }

    template<typename T>
    const T *find() const
    {
        auto it = _findIterator<T>();
        if (it == _components.end())
        {
            return nullptr;
        }
        auto &entry = *it;
        auto &component = entry.component;
        return std::any_cast<T>(&component);
    }

    template<typename T>
    bool has() const
    {
        return find<T>() != nullptr;
    }

private:
    template<typename T>
    auto _findIterator()
    {
        return std::find(_components.begin(), _components.end(), std::type_index(typeid(T)));
    }

    template<typename T>
    auto _findIterator() const
    {
        return std::find(_components.begin(), _components.end(), std::type_index(typeid(T)));
    }

    struct Entry
    {
        std::type_index type;
        std::any component;

        bool operator==(std::type_index typeTest) const noexcept
        {
            return type == typeTest;
        }
    };

    std::vector<Entry> _components;
};
}
