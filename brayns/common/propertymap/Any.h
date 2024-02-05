/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <cassert>
#include <typeindex>

#include <boost/any.hpp>

namespace brayns
{
/**
 * @brief A generic object to holds any data type (wrapper)
 *
 */
class Any
{
public:
    /**
     * @brief Construct an empty any (ie isEmpty() = true)
     *
     */
    Any() = default;

    /**
     * @brief Construct a new Any object holding the given type.
     *
     * @tparam T The type of the object to store.
     * @param value The value to store.
     */
    template <typename T>
    Any(T&& value)
        : _data(std::forward<T>(value))
    {
    }

    /**
     * @brief Check if an object is contained in the Any instance.
     *
     * @return true The object is empty (default constructed).
     * @return false An object of any type is stored in the instance.
     */
    bool isEmpty() const { return _data.empty(); }

    /**
     * @brief Get the type of the stored object.
     *
     * @return std::type_index Type of stored object, typeid(void) if empty.
     */
    std::type_index getType() const { return _data.type(); }

    /**
     * @brief Shorthand to check if getType() == typeid(T).
     *
     * @tparam T The desired type.
     * @return true The stored value type is T.
     * @return false The stored value is not T.
     */
    template <typename T>
    bool is() const
    {
        return getType() == typeid(T);
    }

    /**
     * @brief Access the stored type as const reference.
     *
     * @tparam T The type of the result, MUST be the same as the type of stored
     * object (undefined behavior otherwise).
     * @return const T& A const reference on the stored object.
     */
    template <typename T>
    const T& as() const
    {
        assert(is<T>());
        return *boost::any_cast<T>(&_data);
    }

    /**
     * @brief Access the stored type as mutable reference.
     *
     * @tparam T The type of the result, MUST be the same as the type of stored
     * object (undefined behavior otherwise).
     * @return T& A mutable reference on the stored object.
     */
    template <typename T>
    T& as()
    {
        assert(is<T>());
        return *boost::any_cast<T>(&_data);
    }

    /**
     * @brief Shorthand to check if the Any is empty.
     *
     * @return true An object is stored inside the instance.
     * @return false The Any is empty.
     */
    explicit operator bool() const { return !isEmpty(); }

private:
    boost::any _data;
};
} // namespace brayns