/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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
#include <unordered_map>

namespace brayns
{
class Components
{
public:
    /**
     * @brief Adds a component to this container. If the component already exists, it is replaced.
     *
     * @tparam T Type of the component to add.
     * @tparam Args Constructor variadic args.
     * @param args Optional variadic args to build of the component.
     * @return T& The newly added component.
     */
    template<typename T, typename... Args>
    T &add(Args &&...args)
    {
        static_assert(std::is_same_v<std::decay_t<T>, T>, "Only decayed components allowed");

        auto component = std::make_unique<Component<T>>();
        auto &data = component->data;
        data = T{std::forward<Args>(args)...};

        _components[typeid(T)] = std::move(component);

        return data;
    }

    /**
     * @brief Attempts to retrieve the component type from the container. If not present, a new one will be added.
     *
     * @tparam T Type of the component to retrieve or add.
     * @tparam Args Constructor variadic args.
     * @param args Optional variadic args to build the component if needed.
     * @return T& The retrieved or added component.
     */
    template<typename T, typename... Args>
    T &getOrAdd(Args &&...args)
    {
        if (auto component = find<T>())
        {
            return *component;
        }
        return add<T>(std::forward<Args>(args)...);
    }

    /**
     * @brief Retrieves a component type from the container. Attempting to get a component that is not present results
     * in undefined behaviour.
     *
     * @tparam T Type of the component to get.
     * @return T& The retrieved component.
     */
    template<typename T>
    T &get()
    {
        auto component = find<T>();
        assert(component);
        return *component;
    }

    /**
     * @copydoc Components::get()
     */
    template<typename T>
    const T &get() const
    {
        auto component = find<T>();
        assert(component);
        return *component;
    }

    /**
     * @brief Searches the container for a type of component.
     *
     * @tparam T Type of component to search.
     * @return T* Pointer to the found component, or nullptr if not present.
     */
    template<typename T>
    T *find()
    {
        return _find<T>();
    }

    /**
     * @copydoc Components::find()
     */
    template<typename T>
    const T *find() const
    {
        return _find<T>();
    }

    /**
     * @brief Test if a component type is present in the container.
     *
     * @tparam T Type of component to check for.
     * @return true if the component is present, false otherwise.
     */
    template<typename T>
    bool has() const
    {
        return find<T>() != nullptr;
    }

private:
    template<typename T>
    T *_find() const
    {
        auto it = _components.find(typeid(T));
        if (it == _components.end())
        {
            return nullptr;
        }
        auto &component = it->second;
        auto &cast = static_cast<Component<T> &>(*component);
        return &cast.data;
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

    std::unordered_map<std::type_index, std::unique_ptr<IComponent>> _components;
};
}
