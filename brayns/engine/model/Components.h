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
#include <cassert>
#include <memory>
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

        auto &entry = _components.emplace_back(typeid(T), std::make_unique<Component<T>>());
        auto &component = static_cast<Component<T> &>(*entry.component);
        auto &data = component.data;

        if constexpr (std::is_aggregate_v<T>)
        {
            data = T{std::forward<Args>(args)...};
        }
        else
        {
            data = T(std::forward<Args>(args)...);
        }

        return data;
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
        auto &cast = static_cast<Component<T> &>(*component);
        return &cast.data;
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
        auto &cast = static_cast<Component<T> &>(*component);
        return &cast.data;
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

    struct IComponent
    {
        virtual ~IComponent() = default;
    };

    template<typename T>
    struct Component : public IComponent
    {
        T data;
    };

    struct Entry
    {
        std::type_index type;
        std::unique_ptr<IComponent> component;

        Entry(std::type_index type, std::unique_ptr<IComponent> component)
            : type(type)
            , component(std::move(component))
        {
        }

        bool operator==(std::type_index typeTest) const noexcept
        {
            return type == typeTest;
        }
    };

    std::vector<Entry> _components;
};
}
